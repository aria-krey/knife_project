#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include "PatternTemplate.h"

using namespace std;

// родительский класс
class Knife {
protected:
    string Material;
    double Length;
    bool Knife_is_sharp;

public:
    Knife(const string& material, double length) : Material(material), Length(length), Knife_is_sharp(false) {}
    virtual ~Knife() {}

    // функция с реализацией
    string Get_material() const { return Material; }
    double Get_length() const { return Length; }
    bool is_sharp() const { return Knife_is_sharp; }

    // виртуальный метод
    virtual void cut() = 0;
    virtual void knife_feature() const = 0;
};

// Класс-наследник "Кухонный нож"
class Kitchen_knife : public Knife {
    string purpose; // назначение ножа
public:
    Kitchen_knife(const string& material, double length, const string& purpose)
        : Knife(material, length), purpose(purpose) {
        Knife_is_sharp = true;
    }
    void cut() override {
        cout << "The kitchen knife is slicing " << purpose << endl;
    }
    void knife_feature() const override {
        cout << "Special purpose " << purpose << endl;
    }
};

// Класс-наследник "Охотничий нож"
class Hunting_knife : public Knife {
    bool presence_saw; // наличие пилы на обузе охотничьего ножа
public:
    Hunting_knife(const string& material, double length, bool presence_saw)
        : Knife(material, length), presence_saw(presence_saw) {
        Knife_is_sharp = true;
    }
    void cut() override {
        cout << "The hunting knife is cutting ";
        if (presence_saw) { cout << " and has the presence of a saw on a burden"; }
        cout << endl;
    }
    void knife_feature() const override {
        cout << "The presence of a saw on a burden: " << (presence_saw ? "yes" : "no") << endl;
    }
};

// Класс-наследний "Складной нож"
class Folding_knife : public Knife {
    string type_lock; // тип замка на ноже
public:
    Folding_knife(const string& material, double length, string type_lock)
        : Knife(material, length), type_lock(type_lock) {
        Knife_is_sharp = true;
    }
    void cut() override {
        cout << "The folding knife with mechanism " << type_lock << " queckly opens and cuts" << endl;
    }
    void knife_feature() const override {
        cout << "The type of lock: " << type_lock << endl;
    }
};

// фабричный метод
enum class Knife_type {
    Kitchen = 1,
    Hunting = 2,
    Folding = 3,
    Undefined = 0
};

Knife *Create_knife(Knife_type type) {
    Knife *new_knife = nullptr;
    if (type == Knife_type::Kitchen) {
        new_knife = new Kitchen_knife("steel", 20.0, "vegetables");
    }
    else if (type == Knife_type::Hunting) {
        new_knife = new Hunting_knife("damascus", 25.0, true);
    }
    else if (type == Knife_type::Folding) {
        new_knife = new Folding_knife("titanium", 10.0, "liner");
    }
    return new_knife;
}

// функция для обработки контейнера через итератор
void process_knives(Iterator<Knife*>* iterator)
{
    for (iterator->First(); !iterator->IsDone(); iterator->Next()) {
        Knife* current = iterator->GetCurrent();
        cout << "\nMaterial: " << current->Get_material() << endl;
        cout << "Length: " << current->Get_length() << " cm" << endl;
        current->cut();
        current->knife_feature();
    }
}

// Декоратор для фильтрации по материалу ножа
class Material_decorator : public IteratorDecorator<Knife*> {
private:
    string target_material;
public:
    Material_decorator(Iterator<Knife*>* it, const string& material) : IteratorDecorator<Knife*>(it), target_material(material) {}
    void First() override
    {
        It->First();
        while(!It->IsDone() && It->GetCurrent()->Get_material() != target_material) {
            It->Next();
            }
    }

    void Next() override
    {
        do { It->Next(); }
        while(!It->IsDone() && It->GetCurrent()->Get_material() != target_material);
    }
};

// Декоратор для фильтрации острых ножей
class Sharp_decorator : public IteratorDecorator<Knife*> {
public:
    Sharp_decorator(Iterator<Knife*>* it) : IteratorDecorator<Knife*>(it) {}

    void First() override
    {
        It->First();
        while(!It->IsDone() && !It->GetCurrent()->is_sharp()) {
            It->Next();
        }
    }

    void Next() override
    {
        do { It->Next(); }
        while(!It->IsDone() && !It->GetCurrent()->is_sharp());
    }
};

// Декоратор для фильтрации по длине ножа
class Length_decorator : public IteratorDecorator<Knife*> {
private:
    double Min_length;
public:
    Length_decorator(Iterator<Knife*>* it, double min_length)
        : IteratorDecorator<Knife*>(it), Min_length(min_length) {}

    void First() override
    {
        It->First();
        while(!It->IsDone() && It->GetCurrent()->Get_length() < Min_length) {
            It->Next();
        }
    }

    void Next() override
    {
        do { It->Next(); }
        while(!It->IsDone() && It->GetCurrent()->Get_length() < Min_length);
    }
};

int main() {
    srand(time(0)); // инициализация генератора псевдослучайных чисел

    // демонстрация фабричного метода
    cout << "What kind of knife to create (1 - kitchen, 2 - hunting, 3 - folding)?" << endl;
    int choice;
    cin >> choice;
    Knife_type type = static_cast<Knife_type>(choice);

    Knife* knife = Create_knife(type);
    if (knife) {
        knife->cut();
        knife->knife_feature();
        delete knife;
    }
    cout << endl << endl;

    // задание с контейнерами
    size_t N;
    cout << "Enter the number of knives: ";
    cin >> N;

    // 1 контейнер стек
    StackClass<Knife*> knife_stack;
    for (size_t i=0; i<N; i++) {
        int type = rand() % 3 + 1;
        knife_stack.Push(Create_knife(static_cast<Knife_type>(type)));
    }
    cout << "Stack knives (" << knife_stack.Size() << "):" << endl;
    Iterator<Knife*>* stack_Iter = knife_stack.GetIterator();
    process_knives(stack_Iter);
    delete stack_Iter;
    cout << endl;

    // 2 контейнер массив
    ArrayClass<Knife*> knife_array;
    for (size_t i=0; i<N; i++) {
        int type = rand() % 3 + 1;
        knife_array.Add(Create_knife(static_cast<Knife_type>(type)));
    }
    cout << "Array of knives (" << knife_array.Size() << "):" << endl;
    Iterator<Knife*>* array_Iter = knife_array.GetIterator();
    process_knives(array_Iter);
    delete array_Iter;
    cout << endl;

    // Демонстрация декораторов
    cout << "\nSharp knives only:" << endl;
    Iterator<Knife*>* sharp_iter = new Sharp_decorator(knife_array.GetIterator());
    process_knives(sharp_iter);
    delete sharp_iter;

    cout << "\nSteel knives only:" << endl;
    Iterator<Knife*>* steel_iter = new Material_decorator(knife_array.GetIterator(), "steel");
    process_knives(steel_iter);
    delete steel_iter;

    cout << "\nKnives longer than 15cm:" << endl;
    Iterator<Knife*>* long_iter = new Length_decorator(knife_array.GetIterator(), 15.0);
    process_knives(long_iter);
    delete long_iter;

    // Демонстрация адаптера для STL-контейнера
    list<Knife*> knife_list;
    for(size_t i=0; i<N; i++) {
        int type = rand() % 3 + 1;
        knife_list.push_back(Create_knife(static_cast<Knife_type>(type)));
    }

    cout << "\nSTL list with adapter:" << endl;
    Iterator<Knife*>* adapted_iter = new ConstIteratorAdapter<list<Knife*>, Knife*>(&knife_list);
    process_knives(adapted_iter);
    delete adapted_iter;

    return 0;
}
