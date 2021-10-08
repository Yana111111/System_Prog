
/*************************************************************************
   LAB 1

	Edit this file ONLY!

*************************************************************************/


#include <stdio.h>
#include "dns.h"


const int SIZE_OF_DB = 12838;//size of data base with hosts
const int MAX_LENGTH_OF_HOST_NAME = 101;


typedef struct HASH
{
	IPADDRESS ip;
	char* str;
	struct HASH* next;
}HASH;

const size_t PAIR_SIZE = sizeof(HASH);

DNSHandle InitDNS()
{
	DNSHandle hDNS = (unsigned int)(HASH*)calloc(SIZE_OF_DB, PAIR_SIZE);
	if ((HASH*)hDNS != NULL)
	{
		return hDNS;
	}
	return INVALID_DNS_HANDLE;
}

/*
* hash function
*/
unsigned int HashFunc(const char* str)
{
	unsigned int temp2;
	unsigned int answer = 0;
	int n = strlen(str);
	for (int i = 0; i < n; ++i) {
		answer *= 39;
		if (isalpha(str[i])) {
			temp2 = (int)str[i] - 97;
		}
		else {
			temp2 = (int)str[i] - 19;
		}
		answer += temp2;
		answer %= SIZE_OF_DB;
	}
	return answer;
}

/*
* Function that adds element to hash table
*/
void AddToTable(DNSHandle hDNS, char* str, IPADDRESS ip)
{
	unsigned int lengthOfHost = strlen(str);
	unsigned int indexOfElement = HashFunc(str);

	if (((HASH*)hDNS)[indexOfElement].str == NULL)
	{
		((HASH*)hDNS)[indexOfElement].ip = ip;
		((HASH*)hDNS)[indexOfElement].str = (char*)malloc(lengthOfHost + 1);
		strcpy(((HASH*)hDNS)[indexOfElement].str, str);
		((HASH*)hDNS)[indexOfElement].next = NULL;
	}
	else
	{
		HASH* p;
		p = (HASH*)malloc(PAIR_SIZE);
		p->ip = ip;
		p->str = (char*)malloc(lengthOfHost + 1);
		strcpy(p->str, str);
		p->next = ((HASH*)hDNS)[indexOfElement].next;
		((HASH*)hDNS)[indexOfElement].next = p;
	}

}


void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath)
{
	FILE* fInput = fopen(hostsFilePath, "r");
	char* string = (char*)malloc(MAX_LENGTH_OF_HOST_NAME);
	unsigned int num1 = 0, num2 = 0, num3 = 0, num4 = 0;

	if (fInput == NULL)
	{
		return;
	}

	while (fscanf_s(fInput, "%d.%d.%d.%d %s", &num1, &num2, &num3, &num4, string, 100) != EOF)
	{
		IPADDRESS ip = (num1 & 0xFF) << 24 | (num2 & 0xFF) << 16 | (num3 & 0xFF) << 8 | (num4 & 0xFF);
		AddToTable(hDNS, string, ip);
	}

	free(string);
	fclose(fInput);
}


IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName)
{
	unsigned int index = HashFunc(hostName);
	HASH* it = &((HASH*)hDNS)[index];
	while (it != NULL && strcmp(it->str, hostName) != 0)
	{
		it = it->next;
	}
	if (it != NULL)
	{
		return it->ip;
	}
	return INVALID_IP_ADDRESS;
}


void ShutdownDNS(DNSHandle hDNS)
{
	for (int i = 0; i < SIZE_OF_DB; i++)
	{
		HASH* q = &(((HASH*)hDNS)[i]);
		if (q != NULL)
		{
			if (q->str != NULL)
			{
				free(q->str);
			}
			q = q->next;
			while (q != NULL)
			{
				HASH* p = q;
				q = q->next;
				if (p->str != NULL)
				{
					free(p->str);
				}
				free(p);
			}
		}
	}

	if ((HASH*)hDNS != NULL)
	{
		free((HASH*)hDNS);
	}
}