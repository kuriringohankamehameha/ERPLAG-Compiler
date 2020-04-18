#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "activation_record.h"

extern ActivationRecord* activation_record;

int main() {
    activation_record = calloc (10, sizeof(ActivationRecord));
    add_variable_activation("Hello", 1);
    add_variable_activation("Boyo", 1);
    add_variable_activation("Lol", 0);
    print_activation_records(2);
    return 0;
}
