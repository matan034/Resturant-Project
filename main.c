#include "targ3_header.h"
void main()
{
	int func,tables,new_quantity,table_num,last_table;
	char name[50];
	product_manage kitchen;
	table_manage table_manager;
	FILE* manot,*instructions;
	if (!(manot = fopen("manot.txt", "r")))//open file for read and check if succeed
		Error_Msg("Could not open file");
	if (!(instructions = fopen("instructions.txt", "r")))//open file for read and check if succeed
		Error_Msg("Could not open file");
	fscanf(instructions, "%d", &tables);//get amount of tables
	CreateTables(&table_manager, tables);//create link list of tables
	while (fscanf(instructions, "%d", &func)!=EOF)//get what function to start
	{
		switch (func)
		{
		case 1://create products link list
			CreateProducts(manot, &kitchen);//create our products and recieve how many products we created
			break;
		case 2:
			fscanf(instructions, "%s %d", name, &new_quantity);
			AddItems(&kitchen, name, new_quantity);
			break;
		case 3:
			fscanf(instructions, "%d %s %d", &table_num, name, &new_quantity);
			OrderItem(&table_manager, &kitchen,table_num, name, new_quantity);
			break;
		case 4:
			fscanf(instructions, "%d %s %d", &table_num, name, &new_quantity);
			RemoveItem(&table_manager, table_num, name, new_quantity);
			break;
		case 5:
			fscanf(instructions, "%d", &table_num);
			last_table = RemoveTable(table_num, &table_manager, &kitchen);//last_table checks if we are on the last table. recieve 0 if its the last table recieve 1 if its not 
			if (last_table)//when its last table free the memory of the lists
			{
				FreeTable(&table_manager);//free all the tables
				Free_kitchen(&kitchen);//free everything in the kitchen
			}
			break;
		}
	}
	fclose(manot);
	fclose(instructions);
}

