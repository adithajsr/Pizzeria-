//
// Assignment 2 21T3 COMP1511: CS Pizzeria
// pizzeria.c
//
// This program was written by ADITHA JAYASURIYA (z5363611)
// on 08/11/2021
//
// A program which can be used to manage a pizzeria. Includes keeping track of 
//orders and their ingredients, calculating the profit made, and resetting each day.
//
// Version 1.0.0: Release

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pizzeria.h"

// TODO: add your own #defines here.

struct ingredient {
    char ingredient_name[MAX_STR_LENGTH];
    int amount;
    double price;
    struct ingredient *next;
};

struct order {
    char customer[MAX_STR_LENGTH];
    char pizza_name[MAX_STR_LENGTH];
    double price;
    int time_allowed;
    struct ingredient *ingredients;
    struct order *next;
};

struct pizzeria {
    struct order *orders;
    struct order *selected_order;
    struct ingredient *stock;
};


//////////////////////////////////////////////////////////////////////



// Prints a single order
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
);

// Prints an ingredient given the name, amount and price in the required format.
// This will be needed for stage 2.
void print_ingredient(char *name, int amount, double price);

////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////


struct pizzeria *create_pizzeria() {
    // Allocates memory to store a `struct pizzeria` and returns a pointer to
    // it. The variable `new` holds this pointer!
    struct pizzeria *new = malloc(sizeof(struct pizzeria));
    new->orders = NULL;
    new->selected_order = NULL;
    new->stock = NULL;
    return new;
}

//Adds an element to the struct order
int add_order(
    struct pizzeria *pizzeria,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
) {
    
    //accounting for invalid inputs
    if (price < 0) {
        return INVALID_PRICE;
    }

    if (time_allowed <= 0) { 
        return INVALID_TIME;
    }

    //creating a pointer to struct order which contains the values given by user
    struct order *new_order = malloc(sizeof(struct order));
    strcpy(new_order->customer, customer);
    strcpy(new_order->pizza_name, pizza_name);
    new_order->price = price;
    new_order->time_allowed = time_allowed;
    new_order->ingredients = NULL;
    new_order->next = NULL;

    //creating a pointer to struct order to traverse pizzeria->orders
    struct order *current = pizzeria->orders;

    //if no elements in list, new_order becomes first element
    if (current == NULL) { 
        pizzeria->orders = new_order;
    }
   
    //else, loop until end of list and insert new_order here
    else {
        while (current->next != NULL) { //why current != NULL doesn't work?
            current = current->next;
        }
        current->next = new_order;
    }
    return SUCCESS;
}

//Prints elements in struct order
void print_all_orders(struct pizzeria *pizzeria) {

    struct order *current = pizzeria->orders;
    int num = 1;

    //loop to print out each element of the linked list
    while (current != NULL) {
        print_order(num, current->customer, current->pizza_name, current->price, current->time_allowed);
        num++;
        current = current->next;
   }

    //Prints currently selected order (current location of pizzeria->selected_order)
    //accessed in function select_next_order
    print_selected_order(pizzeria);
    return;
}

//Determines the shortest time_allowed among the orders
int next_deadline(struct pizzeria *pizzeria) {
    
    struct order *current = pizzeria->orders;
    
    if (pizzeria->orders != NULL) {
        //assigning current time_allowed to a variable
        int holder = current->time_allowed;
        //loop which tests whether the succeeding time_allowed is less than the 
        //previous time_allowed (stored in holder). If so, holder becomes current
        //time_allowed. Returns holder which is lowest time_allowed.
        while (current->next != NULL) {    
            current = current->next;
            if (current->time_allowed < holder) {
                holder = current->time_allowed;
            }           
        }
        return holder;
    }
    
    //in case no elements in list
    else {
        return INVALID_CALL;
    }
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

//Selects the next order in the list - this is useful for different functions, 
//such as cancelling an order. 
void select_next_order(struct pizzeria *pizzeria) {

    
    struct order *current = pizzeria->selected_order;
    //goes to next order in selected_order every time this function is called
    if (current != NULL) {
        current = current->next;
        pizzeria->selected_order = current;
    }
    
    else {
        current = pizzeria->orders;
        pizzeria->selected_order = current;
    }
   

    return;
}

//Selects the previous order in the list - this is useful for different functions, 
//such as cancelling an order. 
void select_previous_order(struct pizzeria *pizzeria) {

    struct order *current = pizzeria->selected_order;
    struct order *head = pizzeria->orders;

    //if the selected order is first one in the list, then after using this 
    //function it becomes NULL
    if (current == head) {
        pizzeria->selected_order = NULL;
    }
    
    //if current isn't NULL, increase head until the element before current. Then
    //set the currently selected_order to head.
    else if (current != NULL) {
        while (head->next != current) {
            head = head->next;
        }
        pizzeria->selected_order = head;
    }

    
    //if current is NULL, we set it equal to the first element in the list, then
    //increase it until it is the last element in the list. Then set the currently
    //selected_order to current. 
    else {
        current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        pizzeria->selected_order = current;
    }

    return;
}

//Prints the currently selected order
void print_selected_order(struct pizzeria *pizzeria) {
    
    if (pizzeria->selected_order == NULL) {
        printf("\nNo selected order.\n");
    }
    
    //way to simplify pizzeria->selected_order...? quite long otherwise
    else {
        printf("\nThe selected order is %s's %s pizza ($%.2lf) due in %d minutes.\n", 
        pizzeria->selected_order->customer, pizzeria->selected_order->pizza_name, 
        pizzeria->selected_order->price, pizzeria->selected_order->time_allowed);
        
        struct ingredient *current = pizzeria->selected_order->ingredients;
        while (current != NULL) {
            print_ingredient(current->ingredient_name, current->amount, current->price);
            current = current->next;
        }
    }
}

//Adds an ingredient to the currently selected order in alphabetical order
int add_ingredient(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {
    //assigning values to new_ingredient as given by the user
    struct ingredient *new_ingredient = malloc(sizeof(struct ingredient));

    strcpy(new_ingredient->ingredient_name, ingredient_name);
    new_ingredient->amount = amount;
    new_ingredient->price = price;
    new_ingredient->next = NULL;

    if (new_ingredient->price < 0) {
        free(new_ingredient);
        return INVALID_PRICE;
    }

    if (new_ingredient->amount < 0) {
        free(new_ingredient);
        return INVALID_AMOUNT;
    }

    if (pizzeria->selected_order == NULL) {
        free(new_ingredient);
        return INVALID_ORDER;
    }

    //pointer head to keep track of where in the list we are
    struct ingredient *current = pizzeria->selected_order->ingredients;
    
    //if no ingredients, new_ingredient becomes first element in list
    if (current == NULL || strcmp(new_ingredient->ingredient_name, current->ingredient_name) < 0) {
        new_ingredient->next = current;
        pizzeria->selected_order->ingredients = new_ingredient;
        }

    //otherwise, loop until end of list, and insert new_ingredient there
    else {
        while (current->next != NULL && strcmp(new_ingredient->ingredient_name, current->next->ingredient_name) > 0) {
            current = current->next;
        }
        if (strcmp(new_ingredient->ingredient_name, current->ingredient_name) == 0) {
            current->amount += new_ingredient->amount;
            free(new_ingredient);
        }

        else if (current->next != NULL && strcmp(new_ingredient->ingredient_name, current->next->ingredient_name) == 0) {
            current = current ->next;
            current->amount += new_ingredient->amount;
            free(new_ingredient);
        }


        else {
            new_ingredient->next = current->next;
            current->next = new_ingredient;
        }
    }
    


    return SUCCESS;
}

//Calculates the total profit one order will net
double calculate_total_profit(struct pizzeria *pizzeria) {
    

    if (pizzeria->selected_order == NULL) {
        return INVALID_ORDER;
    }
    if (pizzeria->selected_order->ingredients == NULL) {
        return INVALID_ORDER;
    }

    //cost of the first ingredient - the other ingredients' prices will be added
    //to this one in the loop below
    double total_expenses = pizzeria->selected_order->ingredients->price * 
    pizzeria->selected_order->ingredients->amount;
    double total_revenue = pizzeria->selected_order->price;
    struct ingredient *current = pizzeria->selected_order->ingredients;
    current = current->next;

    
    //loop which adds up the total cost of all of the ingredients
    while (current != NULL) {
        double expenses = current->price * current->amount;
        total_expenses += expenses;
        current = current->next;
    }
    
    double profit = total_revenue - total_expenses;
   

    return profit;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

//Removes an order from the list by 'freeing' the memory allocated to it
int cancel_order(struct pizzeria *pizzeria) {

    if (pizzeria->selected_order == NULL) {
        return INVALID_ORDER;
    }

    struct order *head = pizzeria->orders;
    struct order *current = head;    
    
    //control case for when the order being cancelled is the only order in the list
    if (pizzeria->selected_order == head && head->next == NULL) {
        pizzeria->orders = NULL;
        pizzeria->selected_order = NULL;
        struct order *temp = head;
        head = NULL;
        free(temp);
    }
    
    //control case for when the order being cancelled is the first order in a list
    //of two
    else if (pizzeria->selected_order == head && head->next != NULL) {
        pizzeria->selected_order = pizzeria->selected_order->next;
        pizzeria->orders = pizzeria->orders->next;
        struct order *temp = head;
        head = head->next;
        free(temp);

    }
    
    //loops until it finds the selected order, then frees it. 
    else {
        while (current->next != pizzeria->selected_order) {
            current = current->next;
        }

        pizzeria->selected_order = pizzeria->selected_order->next;
        
        struct order *temp = current->next;
        current->next = current->next->next;
        free(temp);    
    }
   

    return SUCCESS;
}

//Frees all the memory allocated in this program 
void free_pizzeria(struct pizzeria *pizzeria) {

    struct order *head_o = pizzeria->orders;
    
    //loop which frees all ingredients in an order, then frees that order. repeats
    //with all orders. 
    while (head_o != NULL) {
        struct ingredient *head_i = head_o->ingredients;
        //if no ingredients in order, free order
        if (head_i == NULL) {
            struct order *temp_o = head_o;
            head_o = head_o->next;
            free(temp_o);       
        }
        //if there are ingredients in order, free ingredients one by one 
        else {
            while (head_i != NULL) {
                struct ingredient *temp_i = head_i;
                head_i = head_i->next;
                free(temp_i);
                
            }
            head_o->ingredients = NULL;
        }
    }

    struct ingredient *head_s = pizzeria->stock;
   
   //frees ingredients in stock 
    while (head_s != NULL) {
        struct ingredient *temp_s = head_s;
        head_s = head_s->next;
        free(temp_s);
    }
       
    free(pizzeria);

    return;
}

//Adds ingredients to stock
int refill_stock(
    struct pizzeria *pizzeria,
    char *ingredient_name,
    int amount,
    double price
) {
     //assigning values to new_ingredient as given by the user
    struct ingredient *new_stock = malloc(sizeof(struct ingredient));
    
    strcpy(new_stock->ingredient_name, ingredient_name);
    new_stock->amount = amount;
    new_stock->price = price;
    new_stock->next = NULL;

    if (new_stock->price <= 0) {
        free(new_stock);
        return INVALID_PRICE;
    }

    if (new_stock->amount <= 0) {
        free(new_stock);
        return INVALID_AMOUNT;
    }


    //pointer head to keep track of where in the list we are
    struct ingredient *current = pizzeria->stock;
    
    //if no ingredients, new_ingredient becomes first element in list
    if (current == NULL || strcmp(new_stock->ingredient_name, current->ingredient_name) < 0) {
        new_stock->next = current;
        pizzeria->stock = new_stock;
        }

    //otherwise, loop until end of list, and insert new_ingredient there
    else {
        while (current->next != NULL && strcmp(new_stock->ingredient_name, current->next->ingredient_name) > 0) {
            current = current->next;
        }
        if (strcmp(new_stock->ingredient_name, current->ingredient_name) == 0) {
            current->amount += new_stock->amount;
            free(new_stock);
        }

        
        else if (current->next != NULL && strcmp(new_stock->ingredient_name, current->next->ingredient_name) == 0) {
            current = current ->next;
            current->amount += new_stock->amount;
            free(new_stock);
        }


        else {
            new_stock->next = current->next;
            current->next = new_stock;
        }
    }
    

    return SUCCESS;
}

//Prints the ingredients in stock
void print_stock(struct pizzeria *pizzeria) {
    
    struct ingredient *current = pizzeria->stock;

    printf("The stock contains:\n");
    

    while (current != NULL) {
        print_ingredient(current->ingredient_name, current->amount, current->price);
        current = current->next;
    }
    

    return;
}

//Determines whether there is enough ingredients in stock to create an order
int can_complete_order(struct pizzeria *pizzeria) {

    if (pizzeria->selected_order == NULL || pizzeria->selected_order->ingredients == NULL) {
        return INVALID_ORDER;
    }

    if (pizzeria->stock == NULL) {
        return INSUFFICIENT_STOCK;
    }

    struct ingredient *current = pizzeria->selected_order->ingredients;
    struct ingredient *current_s = pizzeria->stock;
    
    //loop which does nothing if stock has enough ingredients but exits if 
    //there isn't enough stock to make the order
    while (current != NULL && current_s != NULL) {
        //finds the corresponding ingredient in stock for the ingredient in order
        if (strcmp(current->ingredient_name, current_s->ingredient_name) == 0) {
            //checks if there is enough stock to make the order
            if (current_s->amount < current->amount) {
                return INSUFFICIENT_STOCK;
            }
            current = current->next;
            current_s = current_s->next;
        }

        else {
            current_s = current_s->next;
        }

    }

    //control case for when a user inputs an ingredient in their order which 
    //stock doesn't contain at all
    if (current_s == NULL && current != NULL) {
        return INSUFFICIENT_STOCK;
    }

    //if this function hasn't returned insufficient_stock by now, that means the
    //stock has all the ingredients required to make the order
    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////

//'Completes' an order - freeing an order if it is marked as 'completed' and subtracting
//the ingredients used from the stock. Note there is an error when an ingredient
//amount goes to zero and then one attempts to print the stock.
int complete_order(struct pizzeria *pizzeria) {

    //1. Determine if order can be completed
    //2. If so, remove ingredients from stock 
    //3. Free order

    if (pizzeria->selected_order == NULL || pizzeria->selected_order->ingredients == NULL) {
        return INVALID_ORDER;
    }
    int complete = can_complete_order(pizzeria); 
    struct ingredient *current = pizzeria->selected_order->ingredients;
    struct ingredient *current_s = pizzeria->stock;
    
    
    //tests if there is sufficient stock to create order
    if (complete == SUCCESS) {
        // a loop which subtracts the ordered ingredient amount from the amount
        //in stock and frees the ingredient if the amount is zero. 
        while (current != NULL && current_s != NULL) {
            if (strcmp(current->ingredient_name, current_s->ingredient_name) == 0) {
                current_s->amount -= current->amount;
                //frees ingredient if amount is zero. 
                if (current_s->amount == 0) {
                    struct ingredient *temp_i = current_s;
                    current = current->next;
                    current_s = current_s->next;
                    free(temp_i);
                }
                else {
                    current = current->next;
                    current_s = current_s->next; 
                }
            }
            
            else {
                current_s = current_s->next;
            }

        }
        
    }

    //exits program if insufficient stock to create order
    else {
        return INSUFFICIENT_STOCK;
    }

    //frees the order once it is completed
    struct order *temp = pizzeria->selected_order;
    pizzeria->selected_order = pizzeria->selected_order->next;
    pizzeria->orders = pizzeria->orders->next;
    free(temp);

    
    
    

    return SUCCESS;
}

int save_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

int load_ingredients(struct pizzeria *pizzeria, char *file_name) {

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//               HELPER FUNCTIONS - Add your own here                 //
////////////////////////////////////////////////////////////////////////

// Prints a single order
//
// `print_order` will be given the parameters:
// - `num` -- the integer that represents which order it is sequentially.
// - `customer` -- the name of the customer for that order.
// - `pizza_name` -- the pizza the customer ordered.
// - `price` -- the price the customer is paying for the pizza.
// - `time_allowed` -- the time the customer will wait for the order.
//
// `print_order` assumes all parameters are valid.
//
// `print_order` returns nothing.
//
// This will be needed for Stage 1.
void print_order(
    int num,
    char *customer,
    char *pizza_name,
    double price,
    int time_allowed
) {

    printf("%02d: %s ordered a %s pizza ($%.2lf) due in %d minutes.\n",
        num, customer, pizza_name, price, time_allowed);

    return;
}

// Prints a single ingredient
//
// `print_ingredient` will be given the parameters:
// - `name` -- the string which contains the ingredient's name.
// - `amount` -- how many of the ingredient we either need or have.
// - `price` -- the price the ingredient costs.
//
// `print_ingredient` assumes all parameters are valid.
//
// `print_ingredient` returns nothing.
//
// This will be needed for Stage 2.
void print_ingredient(char *name, int amount, double price) {
    printf("    %s: %d @ $%.2lf\n", name, amount, price);
}





    
