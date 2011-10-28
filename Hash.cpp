#include <cstdio>

#include "D2Helpers.h"
#include "Hash.h"

using namespace std;

char* HashString(char* dataIn, ALG_ID algo)
{
	// set up the crypto environment
	HCRYPTPROV provider;
	HCRYPTHASH hash;
	DWORD lenOut = 0;

	if(!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		return NULL;
	if(!CryptCreateHash(provider, algo, 0, 0, &hash))
	{
		CryptReleaseContext(provider, 0);
		return NULL;
	}

	// now we have a working crypto environment, let's encrypt
	if(!CryptHashData(hash, (BYTE*)dataIn, strlen(dataIn), 0))
	{
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}
	if(!CryptGetHashParam(hash, HP_HASHVAL, NULL, &lenOut, 0))
	{
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}
	
	BYTE* result = new BYTE[lenOut];
	memset(result, 0, lenOut);
	if(!CryptGetHashParam(hash, HP_HASHVAL, result, &lenOut, 0))
	{
		delete[] result;
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}

	// tear down the crypto environment
	CryptDestroyHash(hash);
	CryptReleaseContext(provider, 0);

	// it's the caller's responsibility to clean up the result
	char* szBuffer1 = new char[lenOut*2 + 1], szBuffer2[10] = "";
	memset(szBuffer1, 0, lenOut*2+1);
	for(DWORD i = 0; i < lenOut; i++)
	{
		sprintf_s(szBuffer2, 10, "%.2x", result[i]);
		strcat_s(szBuffer1, lenOut*2+1, szBuffer2);
	}
	delete[] result;
	return szBuffer1;
}

char* HashFile(char* file, ALG_ID algo)
{
	// set up the crypto environment
	HCRYPTPROV provider;
	HCRYPTHASH hash;
	DWORD lenOut = 0;
	if(!CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, 0))
		return NULL;
	if(!CryptCreateHash(provider, algo, 0, 0, &hash))
	{
		CryptReleaseContext(provider, 0);
		return NULL;
	}

	// now we have a working crypto environment, let's encrypt
	// open the file
	FILE* fp = NULL;
	fopen_s(&fp, file, "r");
	if(!fp)
		return NULL;

	// read it to end, adding it to the hash stream
	fseek(fp, 0, SEEK_END);
	unsigned int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* contents = new char[size];
	if(fread(contents, sizeof(char), size, fp) != size && ferror(fp))
	{
		fclose(fp);
		delete[] contents;
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}
	fclose(fp);
	if(!CryptHashData(hash, (BYTE*)contents, size, 0))
	{
		delete[] contents;
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}
	delete[] contents;

	if(!CryptGetHashParam(hash, HP_HASHVAL, NULL, &lenOut, 0))
	{
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}

	BYTE* result = new BYTE[lenOut];
	memset(result, 0, lenOut);
	if(!CryptGetHashParam(hash, HP_HASHVAL, result, &lenOut, 0))
	{
		delete[] result;
		CryptDestroyHash(hash);
		CryptReleaseContext(provider, 0);
		return NULL;
	}

	// tear down the crypto environment
	CryptDestroyHash(hash);
	CryptReleaseContext(provider, 0);

	// it's the caller's responsibility to clean up the result
	char* szBuffer1 = new char[lenOut*2 + 1], szBuffer2[10] = "";
	memset(szBuffer1, 0, lenOut*2+1);
	for(DWORD i = 0; i < lenOut; i++)
	{
		sprintf_s(szBuffer2, 10, "%.2x", result[i]);
		strcat_s(szBuffer1, lenOut*2+1, szBuffer2);
	}
	delete[] result;
	return szBuffer1;
}

char* md5(char* str) { return HashString(str, CALG_MD5); }
char* sha1(char* str) { return HashString(str, CALG_SHA1); }
char* sha256(char* str) { return HashString(str, CALG_SHA_256); }
char* sha384(char* str) { return HashString(str, CALG_SHA_384); }
char* sha512(char* str) { return HashString(str, CALG_SHA_512); }

char* md5_file(char* file) { return HashFile(file, CALG_MD5); }
char* sha1_file(char* file) { return HashFile(file, CALG_SHA1); }
char* sha256_file(char* file) { return HashFile(file, CALG_SHA_256); }
char* sha384_file(char* file) { return HashFile(file, CALG_SHA_384); }
char* sha512_file(char* file) { return HashFile(file, CALG_SHA_512); }
