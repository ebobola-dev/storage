#include <iostream>
#include <fstream>
#include <Windows.h>

#define STORAGE_SIZE 128
#define NAME_SIZE 64
#define MIN_NAME_SIZE 3
#define MAX_POSITIONS 16
#define FILENAME "storage.bin"

using namespace std;

typedef struct Product {
	char name[NAME_SIZE]{};
	float price = 0.0f;
	int quantity = 0;
};

typedef struct ReceiptPosition {
	char productName[NAME_SIZE]{};
	int quantity = 0;
	float price = 0.0f;
};

typedef struct Receipt {
	ReceiptPosition positions[MAX_POSITIONS];
	int positionQuantity = 0;
	float totalPrice = 0.0f;
};


bool readLocalStorage(Product[], int&);
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
	if (readLocalStorage(storage, productsCount)) {
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
bool readLocalStorage(Product storage[], int& productsCount) {
	ifstream file(FILENAME, ios::binary);
	if (!file.is_open()) {
		printf("Не удалось открыть файл %s.\n", FILENAME);
		file.close();
		return false;
	}
	productsCount = 0;
	while (!file.eof()) {
		file.read((char*)&storage[productsCount], sizeof Product);
		if (strlen(storage[productsCount].name) < MIN_NAME_SIZE) {
			continue;
		}
		if (storage[productsCount].price <= 0.0) {
			continue;
		}
		if (storage[productsCount].quantity <= 0) {
			continue;
		}
		productsCount++;
	}
	file.close();
	if (!productsCount) return false;
	return true;
}

void updateLocalStorage(Product storage[], int productsCount) {
	ofstream file(FILENAME, ios::binary);
	for (int i = 0; i < productsCount; i++) {
		file.write((char*) &storage[i], sizeof storage[i]);
	}
	file.close();
}
// --

void printReceipt(Receipt receipt) {
	printf("\n  \tНазвание\tЦена\tКоличество\n");
	for (int i = 0; i < receipt.positionQuantity; i++) {
		ReceiptPosition* receiptPos = &receipt.positions[i];
		printf("%d)\t%s\t%.2f\t%d\n", i + 1, receiptPos->productName, receiptPos->price, receiptPos->quantity);
	}
	printf("Итого %d позиции(й) на %.2f руб.\n", receipt.positionQuantity, receipt.totalPrice);
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

int searchPosition(ReceiptPosition positions[], int positionsCount, char name[NAME_SIZE]) {
	for (int i = 0; i < positionsCount; i++) {
		if (!strcmp(positions[i].productName, name)) {
			return i;
		}
	}
	return -1;
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
		printf("\nВведите название товара(мин. длина - %d): ", MIN_NAME_SIZE);
		cin.getline(inputName, NAME_SIZE);
	} while (strlen(inputName) < MIN_NAME_SIZE);
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
	Receipt newReceipt;

	while (true) {
		
		do {
			printf("\nВведите название товара(Введите 0, чтобы завершить покупки): ");
			cin.getline(name, NAME_SIZE);
		} while ((name[0] != '0' || strlen(name) != 1) && searchProduct(storage, productsCount, name) == -1);

		if (strlen(name) == 1 && name[0] == '0') break;

		int productIndex = searchProduct(storage, productsCount, name);

		int quantity = 0;
		do {
			printf("Введите количество товаров(> 0, <= %d): ", storage[productIndex].quantity);
			cin >> quantity;
			cin.clear();
			cin.get();
		} while (quantity <= 0 || quantity > storage[productIndex].quantity);

		storage[productIndex].quantity -= quantity;

		int positionIndex = searchPosition(newReceipt.positions, newReceipt.positionQuantity, name);
		if (positionIndex != -1) {
			newReceipt.positions[positionIndex].quantity += quantity;
			newReceipt.totalPrice += quantity * storage[productIndex].price;
		}
		else {
			// 
			strcpy_s(newReceipt.positions[newReceipt.positionQuantity].productName, storage[productIndex].name);
			newReceipt.positions[newReceipt.positionQuantity].price = storage[productIndex].price;
			newReceipt.positions[newReceipt.positionQuantity].quantity = quantity;
			//

			newReceipt.totalPrice += quantity * storage[productIndex].price;
			newReceipt.positionQuantity++;
		}

	}

	if (!newReceipt.positionQuantity) {
		printf("Вы ничего не купили\n");
	}
	else {
		printf("Покупка завершена\n");
		printReceipt(newReceipt);
		updateLocalStorage(storage, productsCount);
	}
}