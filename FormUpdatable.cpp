#include "FormUpdatable.h"

FormUpdatable* FormUpdatable::instances[MAX_INSTANCES];
uint8_t FormUpdatable::numInstances = 0;

FormUpdatable::FormUpdatable(char* aName){
    strncpy(name, aName, MAX_NAME_LENGTH);
    name[MAX_NAME_LENGTH] = 0;  // In case strncpy overflowed.
    if(numInstances < MAX_INSTANCES){
      instances[numInstances++] = this;
    }
}

//GET /GET?VALUE=1234 HTTP/1.1
bool FormUpdatable::parse(char* line) {
  bool rv = true;
  if (strlen(line) >= 8 && (strncmp("GET /GET", line, 8) == 0)) {
    char* beg = strtok(line, "?");  // has the GET /GET
    char* mid = strtok(NULL, "=");  // has the name
    char* end = strtok(NULL, " ");  // has the value

    if (beg && mid && end) {
      for (int i = 0; i < numInstances; i++) {
        if (strcmp(instances[i]->name, mid) == 0) {
          instances[i]->parseValue(end);
          rv = true;
          break;
        }
      }
    }
  }
  return rv;
}

void FormUpdatable::printValue(WiFiClient* client) {
  client->print("N/A");
}

void FormUpdatable::printForm(WiFiClient* client) {

  client->print("<form action='/GET'>");
  client->print(name);
  client->print(": ");
  printValue(client);
  client->print(": ");
  client->print("<input type='text' name=");
  client->print(name);
  client->print(">");
  client->print("<input type='submit' value='Submit'>");
  client->print("</form>");
}



void FormUpdatable::listForms(WiFiClient* client) {
  for (int i=0; i<numInstances; i++){
    instances[i]->printForm(client);
  }
}


template<> 
void __attribute__((weak)) FormUpdatableValue<int>::parseValue(char* ptr){
  value = atoi(ptr);
}

template<> 
void __attribute__((weak)) FormUpdatableValue<float>::parseValue(char* ptr){
  value = atof(ptr);
}

template<> 
void __attribute__((weak)) FormUpdatableValue<double>::parseValue(char* ptr){
  value = atof(ptr);
}


