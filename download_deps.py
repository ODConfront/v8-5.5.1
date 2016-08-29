import urllib3
import platform
import os
import threading
import certifi
import tarfile


def Var(str):
	return depVar['vars'][str];

depVar = {
	'vars':{},
	'deps':{},
	'Var':Var
}

execfile( "DEPS", depVar )

http_pool = urllib3.ProxyManager("http://web-proxyhk.oa.com:8080/", cert_reqs='CERT_REQUIRED', ca_certs=certifi.where())

black_list = [
'v8/tools/swarming_client',
'v8/third_party/WebKit/Source/platform/inspector_protocol',
'v8/test/benchmarks/data',
'v8/test/mozilla/data',
'test/test262/data',
'test/simdjs/data'
]

download_temp = 'temp'

def download(url, name, path):
	file_path = ''.join([path,'/',name,'.tar.gz'])
	if not os.path.exists(file_path):
		print 'downloading ' + name
		resp = http_pool.urlopen('GET',url)
		if resp.status==200:
			data_len = len(resp.data)
			print data_len, name
			if data_len > 0:
				with open(file_path,'w') as archive:
					archive.write(resp.data)
					archive.close()

	tar = tarfile.open(file_path, "r:gz")
	dest_dir = name.replace('@','/')
	tar.extractall(dest_dir)
	tar.close()
	
#jts = []

for (dep_name, dep_url) in depVar['deps'].items():
	if dep_name in black_list:
		continue
	sec = dep_url.split("@")
	prefix = sec[0]
	revision = sec[1]
	down_url = ''.join([prefix,'/+archive/',revision,'.tar.gz'])
#	print down_url
#	down_url = 'http://baidu.com'
	#r = http_pool.urlopen('GET',down_url)
	dep_path = dep_name[3:]
	dep_file = dep_path.replace('/','@')
	#j = threading.Thread(target = download, name = dep_path, args = (down_url, dep_file, download_temp))
	#jts.append(j)
	#j.start()
	download(down_url, dep_file, download_temp)

#for j in jts:
	#j.join()
