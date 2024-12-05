#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char *a = "123456";

    // Cấp phát bộ nhớ cho chuỗi mới (bao gồm ký tự null-terminator)
    char *new_a = (char *)malloc(strlen(a) + 1);  // +1 để chứa ký tự '\0'

    if (new_a == NULL) {
        printf("Memory allocation failed!\n");
        return 1;  // Nếu không cấp phát được bộ nhớ, thoát
    }
    strcpy(new_a, a);
    new_a[0] = '0';


    return 0;
}
