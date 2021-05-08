#include "targ3_header.h"
void Free_kitchen(Pmanage kitchen)
{
	product* tempP;
	while(kitchen->head!=NULL)
	{
		tempP=kitchen->head;//set to head
		kitchen->head = kitchen->head->next;//advance head
		free(tempP->name);//free products name
		free(tempP);//free the product
	}
}
void FreeTable(Tmanage table_manage)
{
	Ptable tempT;
	while (table_manage->head != NULL)
	{
		tempT = table_manage->head;//set tempT to be our head so we can delete it
		table_manage->head = table_manage->head->next;//move to next item in the list

		free(tempT);
	}
}
int check_last_order(Tmanage table_manage)
{
	int cnt=0;
	Ptable check;
	check= table_manage->head;
	while (check != NULL)
	{
		if (check->order != NULL)cnt++;//count how many orders are left
		check = check->next;
	}
	return cnt == 1 ? 1 : 0;//if theres only order left  return 1 else return 0
}
int RemoveTable(int table_num,Tmanage table_manage,Pmanage kitchen)
{
	int max=0,tempTableNum=0,flag = 0;//signals that we are not on the last table
	char* maxProduct; 
	FILE* out;
	Ptable tempT;
	product* toDel;
	product* tempP;
	if (!(maxProduct = (char*)malloc(sizeof(char))))//name of the product most sold)
	{
		Error_Msg("Could not allocate memory");
		exit(1);
	}
	if (!(out = fopen("output.txt", "a")))
	{
		printf("could not open file");
		exit(1);
	}
	tempT = table_manage->head;
	tempP = kitchen->head;
	while (tempT->num != table_num)//run on our table list until we reach the table to close
		tempT = tempT->next;
	if (tempT->order != NULL)//check if the table has an order 
	{
		flag = check_last_order(table_manage);//now that we know the table we recieved is not null we will check if its the last order
		fprintf(out, "The bill on table %d is:\n", table_num);
		while (tempT->order != NULL)
		{
			toDel = tempT->order;//save the head of order list
			fprintf(out, "%d %s. \n", toDel->quantity, toDel->name);//print the items on the table
			tempT->order = tempT->order->next;//move head to next product
			free(toDel->name);
			free(toDel);//free the dish
		}
		fprintf(out, "The total price is:%d NIS,please\n", tempT->price);//print total price
		if (flag)//if it is the last table
		{
			while (tempP != NULL)
			{
				if (tempP->daily_sales > max)
				{
					max = tempP->daily_sales;//update the max sold item
					if (!(maxProduct = (char*)realloc(maxProduct, (strlen(tempP->name) + 1 * sizeof(char)))))//realocate memory according for the name of the most sold dish
					{
						Error_Msg("Couldn't allocate memory");
						exit(1);
					}
					strcpy(maxProduct, tempP->name);
				}
				tempP = tempP->next;
			}
			fprintf(out,"the most popular dish today is %s (was orderd %d times)\n", maxProduct, max);
			free(maxProduct);
		}
	}
	else//table doesnt have an order
		fprintf(out, "The table number %d is not ordered yet\n", table_num);
	fclose(out);
	return flag;//return flag to check if we have reached the last table
}
void RemoveItem(Tmanage table_manage, int table_number, char* name, int quantity)
{
	FILE* out;
	Ptable tempT;
	product* tempP,*before,*todelete;
	if(!(out=fopen("output.txt","a")))
	{
		printf("couldnt open out file");
		exit(1);
	}
	tempT = table_manage->head;//set a pointer to our table
	while (tempT->num != table_number)//find the table that wants to remove an item
		tempT = tempT->next;
	tempP = tempT->order;//set our pointer to the head of the items we have on the table
	before = tempP;//initilize before to point to our head
	if (tempP == NULL)//check if theres any items on the table
	{
		fprintf(out,"There is nothing on the table\n");
		fclose(out);
		return;
	}

	while (strcmp(tempP->name, name) != 0)//find the item BEFORE the one we want to delete
	{
		before = tempP;
		tempP = tempP->next;
		if(tempP==NULL)//if we got a name that isnt on the table
		{
			fprintf(out,"%s is not on table\n", name);
			fclose(out);
			return;
		}
	}
	if (quantity <= tempP->quantity)//check if the amount of quantity exists on the table
	{
		tempT->price -= tempP->price * quantity;//update the tables price
		tempP->quantity -= quantity;//update the quantity of the order on the table
		if (tempP->quantity == 0)//check if they returened all the orders of said item
		{
			todelete = tempP;
			if (todelete == tempT->order)//if the item to delete is the head of our link list
				tempT->order = todelete->next;
			else//if the item to delete is not the head of the list
				before->next = todelete->next;//attach to the item after the item we delete
			free(todelete->name);
			free(todelete);//free the item that we are set to delete
			
		}
		fprintf(out,"%d %s was returned to the kitchen from table number %d\n", quantity, name, table_number);
	}
	else
		fprintf(out,"there isnt %d %s on the table\n", quantity, name);
	fclose(out);
}


void OrderItem(Tmanage table_manage, Pmanage kitchen, int table_number, char* name, int quantity)
{
	FILE* out;
	if(!(out=fopen("output.txt","a")))
	{
		printf("couldnt open out file");
		exit(1);
	}
	product* tempP, * new_product;//tempP will point the kitchen list
	Ptable tempT = table_manage->head;//tempT will point to table list
	tempP = kitchen->head;
	if (!check_name(kitchen, name))
	{
		if (check_quantity(quantity))
		{
			while (strcmp(tempP->name, name)!=0)//loop till you reach the name we want to update its quantity
				tempP = tempP->next;
			if (tempP->quantity >= quantity)//check if we have enough of the item in our inventory
			{
				while (tempT->num != table_number)//run on all the tabels till we find the table with same serial number
					tempT = tempT->next;//go to next table
				new_product = Addtotable(tempT, tempP->name, quantity, tempP->price);//recieve junction of what we put on the table 
				tempP->daily_sales += quantity;//update the amount of times and item has been sold
				//add products to the table(link list) in the style of head of list
				new_product->next = tempT->order;
				tempT->order = new_product;
				tempT->price += new_product->price * new_product->quantity;//update the total price on the table
				tempP->quantity -= quantity;//update the quantity of the item in our kitchen
				fprintf(out,"%d %s were added to table number %d\n", quantity, name, table_number);
			}
			else
			fprintf(out,"sorry there isnt enough %s in the kitchen\n", name);
		}
	}
	else
		fprintf(out,"we dont have %s in the kitchen, sorry!\n", name);
	fclose(out);
}

void CreateTables(Tmanage table_manage,int num)
{
	int i;
	Ptable temp;
	//set by defualt to be null
	table_manage->head = NULL;
	table_manage->tail = NULL;
	for (i = 0; i < num; i++)
	{
		if (!(temp = (table*)malloc(sizeof(table))))//create a table(link list node)
		{
			Error_Msg("\nCould not allocate memory");
			exit(1);
		}
		temp->num = i + 1;//number of table
		temp->order = NULL;//initialize order to null(no orders on table)
		temp->next = NULL;//our tail next is always null
		temp->price = 0;
		if (table_manage->head == NULL)//if its the first item in link list
		{
			table_manage->head = temp;//make it first item
			temp->before = NULL;
		}
		else//if its not the first item in the list
		{
			table_manage->tail->next = temp;//attach to previous tail
			temp->before = table_manage->tail;//attach the list to go backward
		}
		table_manage->tail = temp;//update the tail
	}
}
//this func copys an item and we call the function to add that item to the table
product* Addtotable(Ptable t, char* name, int quantity,int price)
{
	product* temp;//make a product
	if (!(temp = (product*)malloc(sizeof(product))))
	{
		Error_Msg("couldnt add product to table");
		exit(1);
	}
	temp->quantity=quantity;//updates it quantity 
	temp->price=price;//update its price
	if (!(temp->name = (char*)malloc(sizeof(char) + 1 * strlen(name))))
	{
		Error_Msg("couldnt add new product name to table");
		exit(1);
	}
	strcpy(temp->name,name);
	return temp;
}

void AddItems(Pmanage kitchen, char* name, int quantity)
{
	FILE* out;
	product* temp;
	if (!(out = fopen("output.txt", "a")))
	{
		fprintf(out,"Could not open file");
		exit(1);
	}
	temp = kitchen->head;//pointer to move in the list
	if (!check_name(kitchen, name))//check if our name is ok(in the list)
	{
		if (check_quantity(quantity))//check if our quantity is ok(not negative)
		{
			while (strcmp(temp->name, name)!=0)//loop till you reach the name we want to update its quantity
				temp = temp->next;
			temp->quantity += quantity;//update quantity
			fprintf(out,"%d %s were added to the kitchen\n", quantity, name);
		}
	}
	else
		fprintf(out,"We dont have %s in our kitchen,sorry\n", name);
	fclose(out);
}
void CreateProducts(FILE* in, Pmanage kitchen)
{
	product* temp;
	FILE *out;
	char temp_name[50];
	if (!(out = fopen("output.txt", "a")))
	{
		printf("couldnt open output file");
		exit(1);
	}
	kitchen->head = NULL;
	kitchen->tail = NULL;
	kitchen->size = 0;
	if (!(temp = (product*)malloc(sizeof(product))))
	{
		Error_Msg("Could not allocate memory");
		exit(1);
	}
	while (fscanf(in, "%s %d %d", temp_name, &temp->quantity, &temp->price)!=EOF)//enter info from text file into temp
	{
		//check if our name (is NOT in the list) and price and quantity are ok (not equal,not neg) if its ok we add to our link list
		if (check_name(kitchen, temp_name) && check_price(temp->price) && check_quantity(temp->quantity))
		{
			if (!(temp->name = (char*)malloc(strlen(temp_name) + 1 * sizeof(char))))
			{
				Error_Msg("Could not allocate memory");
				if (kitchen->size > 0)//only free memory if we have allocated memory
					DeleteProducts(kitchen);
				exit(1);
			}
			strcpy(temp->name, temp_name);//move our temp_name into a product
			temp->daily_sales = 0;
			temp->next = NULL;//the item we add is always last
			if (kitchen->head == NULL)//if its the first item
				kitchen->head = temp;
			else
				kitchen->tail->next = temp;
			kitchen->tail = temp;
			kitchen->size++;//new item has been added
		}
		//create new temp for next item
		if (!(temp = (product*)malloc(sizeof(product))))
		{
			Error_Msg("Could not allocate memory");
			DeleteProducts(kitchen);
			exit(1);
		}
	}
	free(temp);//free the one extra temp we have allocated
	fprintf(out,"The kitchen has been created\n");
	fclose(out);
}
int check_price(int price)
{
	if (price <= 0)
	{
		Error_Msg("Price cannot be negative");
		return 0;//return 0 if price is negative
	}
	return 1;//return 1 if price is positive
}

int check_quantity(int q)
{
	if (q>0) return 1;//return 1 if quantity is valid
	Error_Msg("quantity isn't valid");
	return 0;
}

int check_name(Pmanage kitchen,char* name)
{
	product* temp;
	temp = kitchen->head;
	while (temp)
	{
		if (strcmp(temp->name, name) == 0)
		{
			return 0;//return 0 if strings are identical
			
		}
	temp = temp->next;
	}
	 return 1;//return 1 if the name doesnt exist
}
void Error_Msg(char* s)
{
	FILE* out;
	if (!(out = fopen("output.txt", "a")))
	{
		printf("Could not open file");
		exit(1);
	}
	fprintf(out,"%s\n",s);
	fclose(out);
}
void DeleteProducts(Pmanage kitchen)
{
	product* temp;
	while (kitchen->head != NULL)
	{
		temp = kitchen->head;//set temp to head
		kitchen->head = kitchen->head->next;//move head
		free(temp->name);//free the name
		free(temp);//free head
	}
}
