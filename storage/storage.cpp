#include <iostream>
#include <fstream>

#define STORAGE_SIZE 128
#define NAME_SIZE 64
#define FILENAME "storage.txt"

using namespace std;

typedef struct Product {
	char name[NAME_SIZE];
	float price;
	int quantity;
};

typedef struct Receipt {
	char productName[NAME_SIZE];
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
	Product storage[STORAGE_SIZE];
	int productsCount = 0;


	printf("Чтение сохранённых товаров...\n");
	if (readStorage(storage, productsCount)) {
		printf("Успешно прочитано %d товаров\n", productsCount);
	}
	else {
		printf("Ошибка при чтении товаров\n");
	}

	printf("Сейчас на складе товаров: %d шт.\n", productsCount);
}

// -- Работа с файлом
bool readStorage(Product storage[], int& productsCount) {
	productsCount = 12;
	return true;
}

void updateLocalStorage(Product storage[], int productsCount) {

}
// --

void printReceipt(Receipt receipt) {
	printf("Чек на сумму %.2f руб.:\n", receipt.totalPrice);
	printf("Товар: %s\n", receipt.productName);
	printf("Количество купленного товара: %d\n\n", receipt.quantity);
}

void printStorage(Product storage[], int productsCount) {
	if (productsCount == 0) {
		printf("Склад пуст\n");
		return;
	}
	printf("Список товаров(%d):\n\n", productsCount);
	for (int i = 0; i < productsCount; i++) {
		printf("Название: %s\n", storage[i].name);
		printf("Цена: %.2f руб.\n", storage[i].price);
		printf("Осталось на складе: %d\n\n", storage[i].quantity);
	}
}

int searchProduct(Product storage[], int productsCount, char name[NAME_SIZE]) {
	for (int i = 0; i < productsCount; i++) {
		if (strcmp(storage[i].name, name)) {
			return i;
		}
	}
	return -1;
}

void addProduct(Product storage[], int& productsCount) {
	char inputName[NAME_SIZE]{};
	int productIndex;
	do {
		printf("Введите название товара(мин. длина - 3): ");
		cin >> inputName;
	} while (strlen(inputName) < 3);
	productIndex = searchProduct(storage, productsCount, inputName);
	if (productIndex == -1) {
		if (productsCount == STORAGE_SIZE) {
			printf("Невозможно добавить новый продукт - склад заполен.\n");
			return;
		}
		strcpy(storage[productsCount].name, inputName);
		do {
			printf("Введите цену товара(> 0): ");
			cin >> storage[productsCount].price;
		} while (storage[productsCount].price <= 0.0f);
		do {
			printf("Введите количество товаров(> 0): ");
			cin >> storage[productsCount].quantity;
		} while (storage[productsCount].quantity <= 0);
		productsCount++;
		printf("Продукт успешно добавлен\n");
	}
	else {
		int addedQuantity;
		do {
			printf("Введите количество товаров, которое нужно добавить(> 0): ");
			cin >> addedQuantity;
		} while (addedQuantity <= 0);
		storage[productIndex].quantity += addedQuantity;
		printf("Количество товаров у %s успешно обновлено до %d.\n", storage[productIndex].name, storage[productIndex].quantity);
	}
	updateLocalStorage(storage, productsCount);
}

void sellProduct(Product storage[], int productsCount) {
	char name[NAME_SIZE];
	do {
		printf("Введите название товара(мин. длина - 3): ");
		cin >> name;
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
	} while (quantity <= 0);

	if (storage[productIndex].quantity < quantity) {
		printf("Ошибка продажи товара - на скалде нет такого количества товара.\n");
		printf("Товар \"%s\" - кол-во: %d шт.\n", storage[productIndex].name, storage[productIndex].quantity);
		return;
	}

	Receipt newReceipt;
	strcpy(newReceipt.productName, storage[productIndex].name);
	newReceipt.totalPrice = (float)quantity * storage[productIndex].price;
	newReceipt.quantity = quantity;
	printReceipt(newReceipt);

	storage[productIndex].quantity -= quantity;
	printf("Товар \"%s\" успешно продан в количестве %d шт.,\nна складе осталось %d шт. этого товара.\n", storage[productIndex].name, quantity, storage[productIndex].quantity);
	
}