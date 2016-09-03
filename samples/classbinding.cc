// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libplatform/libplatform.h"
#include "v8.h"

using namespace v8;
using namespace std;

// function
static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1) return;
	HandleScope scope(args.GetIsolate());
	Local<Value> arg = args[0];
	String::Utf8Value value(arg);
	printf("v8::%s\n", *value);
	args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), "logging...", NewStringType::kNormal).ToLocalChecked());
}

Local<String> toStr(const char* str)
{
	return String::NewFromUtf8(Isolate::GetCurrent(), str, NewStringType::kNormal).ToLocalChecked();
}

// protype
class ProtypeTest
{
public:
	static v8::Persistent<v8::Function> constructor;

	static void Init(Isolate * isolate, v8::Local<v8::ObjectTemplate> &exports)
	{
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "ProtypeTest"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		Local<ObjectTemplate> tplProto = tpl->PrototypeTemplate();
		tplProto->Set(toStr("get"), FunctionTemplate::New(isolate, Get));
		tplProto->Set(toStr("set"), FunctionTemplate::New(isolate, Set));

		exports->Set(toStr("ProtypeTest"), tpl);
	}

	ProtypeTest() : _internal(0) {}

	void Set(int i) { _internal = i; }

	int Get() { return _internal; }

	static void New(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		if (args.IsConstructCall()) {
			ProtypeTest* point = new ProtypeTest();
			Local<Object> object = args.This();
			auto ext = External::New(isolate, point);
			point->_external.Reset(isolate, ext);
			//point->_external.SetWeak(point, )
			object->SetInternalField(0, ext);
			args.GetReturnValue().Set(args.This());
		}
	}

	static void Get(const v8::FunctionCallbackInfo<v8::Value>& args) {
		auto self = args.Holder();
		auto i = Local<External>::Cast(self->GetInternalField(0));
		void* ptr = i->Value();
		ProtypeTest* proto = static_cast<ProtypeTest*>(ptr);
		args.GetReturnValue().Set(Number::New(args.GetIsolate(), proto->Get()));
	}

	static void Set(const v8::FunctionCallbackInfo<v8::Value>& args) {
		auto self = args.Holder();
		auto i = Local<External>::Cast(self->GetInternalField(0));
		void* ptr = i->Value();
		ProtypeTest* proto = static_cast<ProtypeTest*>(ptr);
		proto->Set(args[0]->Int32Value());
		args.GetReturnValue().Set(Number::New(args.GetIsolate(), proto->Get()));
	}

private:
	Persistent<External>	_external;
	int						_internal;
};

Persistent<Function> ProtypeTest::constructor;

// global map


int main(int argc, char* argv[]) {
	// Initialize V8.
	V8::GetVersion();
	V8::InitializeICUDefaultLocation(argv[0]);
	V8::InitializeExternalStartupData(argv[0]);
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();

	Isolate::CreateParams create_params;
	create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	Isolate* isolate = Isolate::New(create_params);
	{
		Isolate::Scope isolate_scope(isolate);

		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);

		Local<ObjectTemplate> global = ObjectTemplate::New(isolate);

		global->Set(toStr("log"), FunctionTemplate::New(isolate, LogCallback));
		ProtypeTest::Init(isolate, global);

		Local<Context> context = Context::New(isolate, nullptr, global);
		Context::Scope context_scope(context);


		Local<String> source = toStr("log('WTF');\n"
			"var p = new ProtypeTest();\n"
			"p.set(100);p.get();\n");
		Local<Script> script = Script::Compile(context, source).ToLocalChecked();

		Local<Value> result = script->Run(context).ToLocalChecked();

		String::Utf8Value val(result);
		printf("Result:%s\n", *val);
	}

	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete create_params.array_buffer_allocator;
	return 0;
}
