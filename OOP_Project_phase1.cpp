#include <bits/stdc++.h>

using namespace std;

class ElementNotFound : public exception
{
    string msg;
public:
    explicit ElementNotFound(const string& name)
    {
        msg = "Error: Element " + name + " not found in library";
    }
    const char* what() const noexcept override
    {
        return msg.c_str();
    }
};
class DuplicateElement : public exception
{
    string msg;
public:
    explicit DuplicateElement(const string& name)
    {
        msg = "Error: Resistor " + name + " already exists in the circuit";
    }
    const char* what() const noexcept override
    {
        return msg.c_str();
    }
};
class SyntaxError : public exception
{
    string msg;
public:
    explicit SyntaxError(const string& message = "Error: Syntax error")
    {
        msg = message;
    }
    const char* what() const noexcept override
    {
        return msg.c_str();
    }
};
double parseRes(const string& valueStr)
{
    regex valRegex(R"(^([0-9.]+)(e[+-]?[0-9]+)?(k|K|Meg|M)?$)");
    smatch match;
    if (!regex_match(valueStr, match, valRegex))
        throw SyntaxError("Error: Invalid resistance value format");
    double value = stod(match[1].str() + (match[2].matched ? match[2].str() : ""));
    string suffix = match[3].matched ? match[3].str() : "";
    if (suffix == "k" || suffix == "K")
        value *= 1e3;
    else if (suffix == "M" || suffix == "Meg")
        value *= 1e6;
    if (value <= 0)
        throw SyntaxError("Error: Resistance cannot be zero or negative");
    return value;
}
class Component
{
public:
    virtual string getName() const = 0;
    virtual ~Component() = default;
};
class Resistor : public Component
{
    string name, node1, node2;
    double resistanceValue;
public:
    Resistor(string n, string n1, string n2, double val) : name(move(n)), node1(move(n1)), node2(move(n2)), resistanceValue(val) {}
    string getName() const override
    {
        return name;
    }
    void print() const
    {
        cout << "Resistor " << name << " between " << node1 << " and " << node2 << " with value " << resistanceValue << " Ohms" << endl;
    }
};
class Circuit
{
    vector<Component*> components;
public:
    ~Circuit()
    {
        for (auto c : components)
            delete c;
    }
    void addResistor(string& name, string& node1, string& node2, string& valueStr)
    {
        for (const auto& c : components)
            if (c->getName() == name)
                throw DuplicateElement(name);
        if (name.empty() || name[0] != 'R')
            throw ElementNotFound(name);
        double value = parseRes(valueStr);
        components.push_back(new Resistor(name, node1, node2, value));
    }
    void deleteResistor(string& name)
    {
        for (int i = 0; i < components.size(); ++i)
        {
            if (components[i]->getName() == name)
            {
                delete components[i];
                components.erase(components.begin() + i);
                return;
            }
        }
        throw runtime_error("Error: Cannot delete resistor; component not found");
    }
    void printAll() const
    {
        for (const auto& c : components)
        {
            const Resistor* r = dynamic_cast<const Resistor*>(c);
            if (r) r->print();
        }
    }
};
void handler(Circuit& circuit, string& input)
{
    smatch match;
    regex addRegex(R"(^add\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_:]+)\s+([0-9.eE+-]+[kK]?|[0-9.eE+-]+(Meg|M)?)$)");
    if (regex_match(input, match, addRegex))
    {
        string rawName = match[1].str();
        if (rawName.empty() || rawName[0] != 'R')
        {
            throw ElementNotFound(rawName);
        }
        string name = rawName;
        string node1 = match[2].str();
        string node2 = match[3].str();
        string value = match[4].str();
        circuit.addResistor(name, node1, node2, value);
        cout << "Resistor " << name << " added successfully.\n";
        return;
    }
    regex delRegex(R"(^delete\s+R([A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delRegex))
    {
        string name = "R" + match[1].str();
        circuit.deleteResistor(name);
        cout << "Resistor " << name << " deleted successfully." << endl;
        return;
    }
    throw SyntaxError();
}
int main()
{
    Circuit circuit;
    string line;
    cout << "Enter commands (type 'exit' to quit):\n";
    while (true)
    {
        cout << "> ";
        getline(cin, line);
        if (line == "exit")
            break;
        try
        {
            handler(circuit, line);
        }
        catch (const exception& ex)
        {
            cout << ex.what() << endl;
        }
    }
    cout << endl;
    cout << "Final circuit:" << endl;
    circuit.printAll();
    return 0;
}
