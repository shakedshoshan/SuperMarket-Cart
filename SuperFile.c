#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	if (!fp) {
		printf("Error open supermarket file to write\n");
		return 0;
	}

	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		fclose(fp);
		return 0;
	}

	if (!saveAddressToFile(&pMarket->location, fp))
	{
		fclose(fp);
		return 0;
	}

	if (!writeIntToFile((int)pMarket->sortOpt, fp, "Error write sort option\n"))
	{
		fclose(fp);
		return 0;
	}

	if (!writeIntToFile(pMarket->productCount, fp, "Error write product count\n"))
	{
		fclose(fp);
		return 0;
	}

	for (int i = 0; i < pMarket->productCount; i++)
	{
		if (!saveProductToFile(pMarket->productArr[i], fp))
		{
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket, customersFileName);

	return 1;
}

int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	pMarket->productArr = NULL;


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}


	int opt;
	if (!readIntFromFile(&opt, fp, "Error reading sort option\n"))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}

	pMarket->sortOpt = (eSortOption)opt;

	if (!readIntFromFile(&pMarket->productCount, fp, "Error reading product count\n"))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}

	if (pMarket->productCount > 0)
	{
		pMarket->productArr = (Product**)malloc(pMarket->productCount * sizeof(Product*));
		if (!pMarket->productArr)
		{
			printf("Allocation error\n");
			free(pMarket->name);
			fclose(fp);
			return 0;
		}
	}


	for (int i = 0; i < pMarket->productCount; i++)
	{
		pMarket->productArr[i] = (Product*)calloc(1, sizeof(Product));
		if (!pMarket->productArr[i])
		{
			printf("Allocation error\n");
			for (int j = 0; j < i; j++) //free all that was allocated
				free(pMarket->productArr[j]);
			free(pMarket->productArr);
			free(pMarket->name);
			fclose(fp);
			return 0;
		}
		if (!loadProductFromFile(pMarket->productArr[i], fp))
		{
			for (int i = 0; i < pMarket->productCount; i++) //free all that was allocated
				free(pMarket->productArr[i]);
			free(pMarket->productArr);
			free(pMarket->name);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	return	loadCustomerFromTextFile(pMarket, customersFileName);

}


int	saveCustomerToTextFile(const SuperMarket* pMarket, const char* customersFileName)
{
	FILE* fp;

	fp = fopen(customersFileName, "w");
	if (!fp) {
		printf("Error open customers file to write\n");
		return 0;
	}

	fprintf(fp, "%d\n", pMarket->customerCount);
	for (int i = 0; i < pMarket->customerCount; i++)
	{
		fprintf(fp, "%s\n", pMarket->customerArr[i].name);
	}
	fclose(fp);
	return 1;
}

int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName)
{
	FILE* fp;

	fp = fopen(customersFileName, "r");
	if (!fp) {
		printf("Error open customers file to write\n");
		return 0;
	}
	pMarket->customerArr = NULL;
	fscanf(fp, "%d\n", &pMarket->customerCount);
	if (pMarket->customerCount > 0)
	{
		pMarket->customerArr = (Customer*)malloc(pMarket->customerCount * sizeof(Customer));
		if (!pMarket->customerArr)
		{
			fclose(fp);
			return 0;
		}
		for (int i = 0; i < pMarket->customerCount; i++)
		{
			pMarket->customerArr[i].name = readDynStringFromTextFile(fp);
			if (!pMarket->customerArr[i].name)
			{
				for (int j = 0; j < i; j++)
					free(pMarket->customerArr[j].name);
				free(pMarket->customerArr);
				fclose(fp);
				return 0;
			}
		}
	}

	fclose(fp);
	return 1;
}





int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;

	fp = fopen(fileName, "r");
	int count;
	fscanf(fp, "%d\n", &count);

	pMarket->productArr = (Product**)realloc(pMarket->productArr, (pMarket->productCount + count) * sizeof(Product*));

	Product p;
	for (int i = 0; i < count; i++)
	{
		myGets(p.name, MAX_STR_LEN, fp);
		myGets(p.barcode, MAX_STR_LEN, fp);
		fscanf(fp, "%d %f %d\n", &p.type, &p.price, &p.count);
		pMarket->productArr[pMarket->productCount] = (Product*)calloc(1, sizeof(Product));
		*pMarket->productArr[pMarket->productCount] = p;
		pMarket->productCount++;
	}

	fclose(fp);
	return 1;
}
