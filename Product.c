#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "fileHelper.h"

const char* typeStr[eNofProductType] = { "Fruit Vegtable", "Fridge", "Frozen", "Shelf" };

void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	getBorcdeCode(pProduct->barcode);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	printf("Enter product price and number of items\t");
	scanf("%f %d", &pProduct->price, &pProduct->count);
}

void initProductName(Product* pProduct)
{
	printf("enter product name up to %d chars\n", NAME_LENGTH - 1);
	myGets(pProduct->name, NAME_LENGTH,stdin);
}

void	printProduct(const Product* pProduct)
{
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %10d\n", typeStr[pProduct->type], pProduct->price, pProduct->count);
}

void	printProductV(const void* val)
{
	const Product* pProd = *(const Product**)val;
	printProduct(pProd);
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (!writeStringToFile(pProduct->name, fp, "Error writing product name\n"))
	{
		return 0;
	}
	if (!writeStringToFile(pProduct->barcode, fp, "Error writing product barcode\n"))
	{
		return 0;
	}
	
	if (!writeIntToFile(pProduct->type, fp, "Error writing product type\n"))
		return 0;

	if (fwrite(&pProduct->price, sizeof(float), 1, fp) != 1)
	{
		puts("error writing price");
		return 0;
	}

	if (!writeIntToFile(pProduct->count, fp, "Error writing product count\n"))
		return 0;

	return 1;
}

int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (!readFixSizeStrFromFile(pProduct->name, fp, "Error reading product name\n"))
	{
		return 0;
	}
	if (!readFixSizeStrFromFile(pProduct->barcode, fp, "Error reading product barcode\n"))
	{
		return 0;
	}
	int type;
	if (!readIntFromFile(&type, fp, "Error reading product type\n"))
		return 0;

	pProduct->type = (eProductType)type;

	if (fread(&pProduct->price, sizeof(float), 1, fp) != 1)
	{
		puts("error reading price");
		return 0;
	}

	if (!readIntFromFile(&pProduct->count, fp, "Error reading product count\n"))
		return 0;

	return 1;
}


void getBorcdeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg, "Code should be of %d length exactly\nUPPER CASE letter and digits\nMust have 3 to 5 digits\n", 
		BARCODE_LENGTH);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		printf("Enter product barcode "); 
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts(msg);
			ok = 0;
		}
		else {
			for (int i = 0; i < BARCODE_LENGTH; i++)
			{
				if (!isupper(temp[i]) && !isdigit(temp[i]))
				{
					puts(msg);
					ok = 0;
					break;
				}
				if (isdigit(temp[i]))
					digCount++;
			}
		}
		if (digCount < 3 || digCount > 5)
			ok = 0;
	} while (!ok);

	strcpy(code, temp);
}


eProductType getProductType()
{
	int option;
	printf("\n\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);
	getchar();
	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

int		isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

void	updateProductCount(Product* pProduct)
{
	int count;
	do {
		printf("How many items to add to stock?");
		scanf("%d", &count);
	} while (count < 1);
	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}

int		compareProductByName(const void* var1, const void* var2)
{
	const Product* pProd1 = *(const Product**)var1;
	const Product* pProd2 = *(const Product**)var2;


	return strcmp(pProd1->name, pProd2->name);

}

int		compareProductByBarcode(const void* var1, const void* var2)
{
	const Product* pProd1 = *(const Product**)var1;
	const Product* pProd2 = *(const Product**)var2;

	return strcmp(pProd1->barcode, pProd2->barcode);
}

int		compareProductByType(const void* var1, const void* var2)
{
	const Product* pProd1 = *(const Product**)var1;
	const Product* pProd2 = *(const Product**)var2;

	return pProd1->type - pProd2->type;
}

int		compareProductByPrice(const void* var1, const void* var2)
{
	const Product* pProd1 = *(const Product**)var1;
	const Product* pProd2 = *(const Product**)var2;

	if (pProd1->price > pProd2->price)
		return 1;

	if (pProd1->price < pProd2->price)
		return -1;

	return 0;
}

