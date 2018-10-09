#include <dsa/low/String.h>

#if defined(APP_LINUX) || defined(APP_BSD) || defined(APP_OSX) || defined(APP_IOS) || defined(APP_ANDROID)

#include <ctype.h>
#include <memory/low/Heap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct String_ {
    // self public object
    String self;

    // constructor data

    // private data
    char* string;
};

// vtable
String_VTable* string_vtable;

// link methods
int string_to_int(struct String* self);
long string_to_long(struct String* self);
double string_to_double(struct String* self);
void string_lower(struct String* self);
void string_upper(struct String* self);
void string_reverse(struct String* self);
void string_copy(struct String* self, char* data);
void string_concat(struct String* self, char* data);
void string_cut(struct String* self, int begin, int end);
void string_replace(struct String* self, int begin, int end, char* replace);
tsize string_length(struct String* self);
int string_compare(struct String* self, char* data);
char* string_value(struct String* self);

// local methods
void string_swap(char* char1, char* char2);

// implement methods
void string_swap(char* char1, char* char2) {
    if (*char1 != *char2) {
        *char1 ^= *char2;
        *char2 ^= *char1;
        *char1 ^= *char2;
    }
}

// vtable operators
int string_to_int(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // convert string to int
    int result = atoi(string_->string);

    return result;
}
long string_to_long(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // convert string to long
    long result = atol(string_->string);

    return result;
}
double string_to_double(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // convert string to Double
    double result = atof(string_->string);

    return result;
}
void string_lower(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // convert all Char's to lower
    for (int cursor = 0; cursor < strlen(string_->string); cursor++) {
        string_->string[cursor] = tolower(string_->string[cursor]);
    }
}
void string_upper(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // convert all Char's to upper
    for (int cursor = 0; cursor < strlen(string_->string); cursor++) {
        string_->string[cursor] = toupper(string_->string[cursor]);
    }
}
void string_reverse(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // reverse string
    for (int cursor = 0; cursor <= strlen(string_->string) / 2; cursor++) {
        string_swap((string_->string + cursor), (string_->string + (strlen(string_->string) - 1) - cursor));
    }
}
void string_copy(struct String* self, char* data) {
    struct String_* string_ = (struct String_*)self;

    // copy data to string
    string_->string = heap_realloc(string_->string, strlen(data) + 1);
    strcpy(string_->string, data);
}
void string_concat(struct String* self, char* data) {
    struct String_* string_ = (struct String_*)self;

    // concatenate data to string
    string_->string = heap_realloc(string_->string, strlen(string_->string) + strlen(data) + 1);
    strcat(string_->string, data);
}
void string_cut(struct String* self, int begin, int end) {
    struct String_* string_ = (struct String_*)self;

    // cut data from string
    string_->string = heap_realloc(string_->string, end - begin + 1);
    for (int cursor = begin; cursor <= end; cursor++) {
        string_->string[cursor - begin] = string_->string[cursor];
    }
    string_->string[end - begin + 1] = '\0';
}
void string_replace(struct String* self, int begin, int end, char* replace) {
    struct String_* string_ = (struct String_*)self;

    // split part 1
    String* part_1 = NULL;
    if (begin > 0) {
        part_1 = string_new_cut(string_->string, 0, begin - 1);
    } else {
        part_1 = string_new_copy("");
    }

    // split part 2
    String* part_2 = NULL;
    if (end < string_get_length(string_->string) - 1) {
        part_2 = string_new_cut(string_->string, end + 1, string_get_length(string_->string));
    } else {
        part_2 = string_new_copy("");
    }

    // replace parts
    self->vtable->copy(self, part_1->vtable->value(part_1));
    self->vtable->concat(self, replace);
    self->vtable->concat(self, part_2->vtable->value(part_2));

    // free parts
    string_free(part_1);
    string_free(part_2);
}
tsize string_length(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // compute string length
    tsize result = string_get_length(string_->string);

    return result;
}
int string_compare(struct String* self, char* data) {
    struct String_* string_ = (struct String_*)self;

    // compare string
    int result = string_get_compare(string_->string, data);

    return result;
}
char* string_value(struct String* self) {
    struct String_* string_ = (struct String_*)self;

    // get string
    char* result = string_->string;

    return result;
}

// object allocation and deallocation operators
void string_init() {
    // init vtable
    string_vtable = heap_alloc(sizeof(String_VTable));
    string_vtable->to_int = string_to_int;
    string_vtable->to_long = string_to_long;
    string_vtable->to_double = string_to_double;
    string_vtable->lower = string_lower;
    string_vtable->upper = string_upper;
    string_vtable->reverse = string_reverse;
    string_vtable->copy = string_copy;
    string_vtable->concat = string_concat;
    string_vtable->cut = string_cut;
    string_vtable->replace = string_replace;
    string_vtable->length = string_length;
    string_vtable->compare = string_compare;
    string_vtable->value = string_value;
}
String* string_new() {
    struct String_* string_ = heap_alloc(sizeof(struct String_));

    // set vtable
    string_->self.vtable = string_vtable;

    // set constructor data

    // set private data
    string_->string = NULL;

    return (String*)string_;
}
void string_free(String* string) {
    struct String_* string_ = (struct String_*)string;

    // free private data
    if (string_->string != NULL) {
        heap_free(string_->string);
    }

    // free self
    heap_free(string_);
}
String* string_new_printf(char* format, ...) {
    struct String_* string_ = (struct String_*)string_new();

    // set constructor data

    // set private data
    va_list args;
    va_start(args, format);
    string_->string = heap_alloc(vsnprintf(NULL, 0, format, args) + 1);
    vsprintf(string_->string, format, args);
    va_end(args);

    return (String*)string_;
}
String* string_new_lower(char* value) {
    // init new string then lower
    String* string = string_new_printf("%s", value);
    string->vtable->lower(string);

    return string;
}
String* string_new_upper(char* value) {
    // init new string then upper
    String* string = string_new_printf("%s", value);
    string->vtable->upper(string);

    return string;
}
String* string_new_reverse(char* value) {
    // init new string then reverse
    String* string = string_new_printf("%s", value);
    string->vtable->reverse(string);

    return string;
}
String* string_new_copy(char* value) {
    // init new string then copy
    String* string = string_new_printf("%s", value);

    return string;
}
String* string_new_concat(char* value, char* data) {
    // init new string then concat
    String* string = string_new_printf("%s", value);
    string->vtable->concat(string, data);

    return string;
}
String* string_new_cut(char* value, int begin, int end) {
    // init new string then cut
    String* string = string_new_printf("%s", value);
    string->vtable->cut(string, begin, end);

    return string;
}
String* string_new_replace(char* value, int begin, int end, char* replace) {
    // init new string then cut
    String* string = string_new_printf("%s", value);
    string->vtable->replace(string, begin, end, replace);

    return string;
}

// local string methods
tsize string_get_length(char* value) {
    tsize result = strlen(value);

    return result;
}
int string_get_compare(char* value, char* data) {
    int result = strcmp(value, data);

    return result;
}

#endif