/*

FormUpdatable  --  Update values in running code using HTML forms
     Copyright (C) 2023  David C.

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

     */

#ifndef FORM_UPDATABLE_H
#define FORM_UPDATABLE_H

#include "WiFiS3.h"

#define MAX_INSTANCES 10
#define MAX_NAME_LENGTH 10

class FormUpdatable {

  static FormUpdatable *instances[MAX_INSTANCES];
  static uint8_t numInstances;

public:
  char name[MAX_NAME_LENGTH + 1];
  FormUpdatable(char* aName);

  void printForm(WiFiClient* client);
  virtual void printValue(WiFiClient* client);
  static bool parse(char* line);
  static void listForms(WiFiClient* client);
  virtual void parseValue(char* ptr) = 0;
};

template <typename T>
class FormUpdatableValue : public FormUpdatable {
public:
  T& value;
  FormUpdatableValue(T& val, char* aName) : FormUpdatable(aName), value(val){}

  virtual void printValue(WiFiClient* client);
  virtual void parseValue(char* ptr);
};

template<typename T>
void FormUpdatableValue<T>::printValue(WiFiClient* client) {
  client->print(value);
}

#endif  //FORM_UPDATABLE_H
