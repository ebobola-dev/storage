#include <iostream>
#include <fstream>
#include <Windows.h>

#define STORAGE_SIZE 128
#define NAME_SIZE 64
#define FILENAME "storage.txt"

using namespace std;

typedef struct Product {
	char name[NAME_SIZE]{};
	float price;
	int quantity;
};

typedef struct Receipt {
	char productName[NAME_SIZE]{};
	int quantity;
	float totalPrice;
};


bool readStorage(Product[], int&);
void updateLocalStorage(Product[], int);

void printStorage(Product[], int);
int searchProduct(Product[], int, char[NAME_SIZE]);
void addProduct(Product[], int&);
void sellProduct(Product[], int);

int main() {
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	Product storage[STORAGE_SIZE];
	int productsCount = 0;
	int choice = -1;

	printf("Чтение сохранённых товаров...\n");
	if (readStorage(storage, productsCount)) {
		printf("Успешно прочитано %d товаров\n", productsCount);
	}
	else {
		printf("Ошибка при чтении товаров\n");
		productsCount = 0;
	}

	printStorage(storage, productsCount);

	while (true) {
		do {
			printf("\nВведите 1, чтобы добавить товар, 2 - продать, 3 - посмотреть на список товаров, 0 - выйти: ");
			cin >> choice;
			cin.get();
		} while (choice == -1);

		switch (choice) {
			case 1:
				addProduct(storage, productsCount);
				break;
			case 2:
				sellProduct(storage, productsCount);
				break;
			case 3:
				printStorage(storage, productsCount);
				break;
		}
		if (!choice) {
			break;
		}
		choice = -1;
	}
}

// -- Работа с файлом
bool readStorage(Product storage[], int& productsCount) {
	ifstream file(FILENAME);
	file >> productsCount;
	if (productsCount == 0) {
		return false;
	}

	char lineBuf[NAME_SIZE];
	int realCount = 0;

	char tempName[NAME_SIZE];
	float tempPrice = 0.0;
	int tempQuantity = 0;

	file.getline(lineBuf, NAME_SIZE);
	for (int i = 0; i < productsCount; i++) {
		file.getline(lineBuf, NAME_SIZE);
		file.getline(lineBuf, NAME_SIZE);
		strcpy_s(tempName, lineBuf);
		if (strlen(tempName) < 3) {
			continue;
		}
		file >> tempPrice;
		if (tempPrice <= 0.0) {
			continue;
		}
		file >> tempQuantity;
		if (tempQuantity <= 0) {
			continue;
		}
		strcpy_s(storage[realCount].name, tempName);
		storage[realCount].price = tempPrice;
		storage[realCount].quantity = tempQuantity;
		realCount++;
		file.getline(lineBuf, NAME_SIZE);
	}
	productsCount = realCount;

	file.close();
	return true;
}

void updateLocalStorage(Product storage[], int productsCount) {
	ofstream file(FILENAME);
	file << productsCount << endl << endl;
	for (int i = 0; i < productsCount; i++) {
		file << storage[i].name << endl;
		file << storage[i].price << endl;
		file << storage[i].quantity << endl << endl;
	}
	file.close();
}
// --

void printReceipt(Receipt receipt) {
	printf("\nЧек на сумму %.2f руб.:\n", receipt.totalPrice);
	printf("Товар: %s\n", receipt.productName);
	printf("Количество купленного товара: %d\n\n", receipt.quantity);
}

void printStorage(Product storage[], int productsCount) {
	if (productsCount == 0) {
		printf("\nСклад пуст\n");
		return;
	}
	printf("\nСписок товаров(%d):\n\n", productsCount);
	for (int i = 0; i < productsCount; i++) {
		printf("Название: %s\n", storage[i].name);
		printf("Цена: %.2f руб.\n", storage[i].price);
		printf("Осталось на складе: %d\n\n", storage[i].quantity);
	}
}

int searchProduct(Product storage[], int productsCount, char name[NAME_SIZE]) {
	for (int i = 0; i < productsCount; i++) {
		if (!strcmp(storage[i].name, name)) {
			return i;
		}
	}
	return -1;
}

void addProduct(Product storage[], int& productsCount) {
	char inputName[NAME_SIZE]{};
	int productIndex;
	do {
		printf("\nВведите название товара(мин. длина - 3): ");
		cin.getline(inputName, NAME_SIZE);
	} while (strlen(inputName) < 3);
	productIndex = searchProduct(storage, productsCount, inputName);
	if (productIndex == -1) {
		if (productsCount == STORAGE_SIZE) {
			printf("Невозможно добавить новый продукт - склад заполен.\n");
			return;
		}
		strcpy_s(storage[productsCount].name, inputName);
		do {
			printf("Введите цену товара(> 0): ");
			cin >> storage[productsCount].price;
			cin.clear();
			cin.get();
		} while (storage[productsCount].price <= 0.0f);
		do {
			printf("Введите количество товаров(> 0): ");
			cin >> storage[productsCount].quantity;
			cin.clear();
			cin.get();
		} while (storage[productsCount].quantity <= 0);
		productsCount++;
		printf("Продукт успешно добавлен\n");
	}
	else {
		int addedQuantity;
		do {
			printf("Введите количество товаров, которое нужно добавить(> 0): ");
			cin >> addedQuantity;
			cin.clear();
			cin.get();
		} while (addedQuantity <= 0);
		storage[productIndex].quantity += addedQuantity;
		printf("Количество товаров у %s успешно обновлено до %d.\n", storage[productIndex].name, storage[productIndex].quantity);
	}
	updateLocalStorage(storage, productsCount);
}

void sellProduct(Product storage[], int productsCount) {
	char name[NAME_SIZE];
	do {
		printf("\nВведите название товара(мин. длина - 3): ");
		cin.getline(name, NAME_SIZE);
	} while (strlen(name) < 3);
	int productIndex = searchProduct(storage, productsCount, name);
	if (productIndex == -1) {
		printf("Ошибка продажи товара - товар с именем %s не найнен.\n", name);
		return;
	}

	int quantity = 0;
	do {
		printf("Введите количество товаров(> 0): ");
		cin >> quantity;
		cin.clear();
		cin.get();
	} while (quantity <= 0);

	if (storage[productIndex].quantity < quantity) {
		printf("Ошибка продажи товара - на скалде нет такого количества товара.\n");
		printf("Товар \"%s\" - кол-во: %d шт.\n", storage[productIndex].name, storage[productIndex].quantity);
		return;
	}

	Receipt newReceipt;
	strcpy_s(newReceipt.productName, storage[productIndex].name);
	newReceipt.totalPrice = (float)quantity * storage[productIndex].price;
	newReceipt.quantity = quantity;
	printReceipt(newReceipt);

	storage[productIndex].quantity -= quantity;
	printf("Товар \"%s\" успешно продан в количестве %d шт.,\nна складе осталось %d шт. этого товара.\n", storage[productIndex].name, quantity, storage[productIndex].quantity);
	updateLocalStorage(storage, productsCount);
}