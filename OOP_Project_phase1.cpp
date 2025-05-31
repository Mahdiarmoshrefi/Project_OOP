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

double parseCapValue(const string& val)
{
    double num;
    char suffix = val.back();
    try {
        if (suffix == 'u' || suffix == 'U')
            return stod(val.substr(0, val.size() - 1)) * 1e-6;
        if (suffix == 'n' || suffix == 'N')
            return stod(val.substr(0, val.size() - 1)) * 1e-9;
        if (suffix == 'f' || suffix == 'F')
            return stod(val.substr(0, val.size() - 1));
        return stod(val);
    }
    catch (...)
    {
        throw SyntaxError("Error: Invalid capacitor value");
    }
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

class Node
{
    string id;
    double voltage;
public:
    explicit Node(string id) : id(move(id)), voltage(0.0) {}

    string getID() const
    {
        return id;
    }
    double getVoltage() const
    {
        return voltage;
    }
    void setVoltage(double v)
    {
        voltage = v;
    }

    void print() const {
        cout << "Node " << id << " with voltage: " << voltage << " V" << endl;
    }
};
class Circuit
{
    vector<Component*> components;

    map<string, string> capNode1;
    map<string, string> capNode2;
    map<string, string> capValue;

    vector<Node*> nodes;

    void ensureNodeExists(const string& id)
    {
        for (auto node : nodes)
            if (node->getID() == id)
                return;
        nodes.push_back(new Node(id));
    }
public:
    ~Circuit()
    {
        for (auto c : components)
            delete c;
        for (auto n : nodes)
            delete n;

    }

    void addResistor(string& name, string& node1, string& node2, string& valueStr)
    {
        for (const auto& c : components)
            if (c->getName() == name)
                throw DuplicateElement(name);
        if (name.empty() || name[0] != 'R')
            throw ElementNotFound(name);

        double value = parseRes(valueStr);
        ensureNodeExists(node1);
        ensureNodeExists(node2);

        components.push_back(new Resistor(name, node1, node2, value));
    }

    void deleteResistor(string& name)
    {
        for (int i = 0; i < (int)components.size(); ++i)
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

    void addCapacitor(string& name, string& node1, string& node2, string& value)
    {
        if (name.empty() || name[0] != 'C')
            throw ElementNotFound(name);

        if (capNode1.count(name))
            throw DuplicateElement(name);

        double numericValue = parseCapValue(value);
        if (numericValue <= 0)
            throw SyntaxError("Error: Capacitance cannot be zero or negative");

        ensureNodeExists(node1);
        ensureNodeExists(node2);

        capNode1[name] = node1;
        capNode2[name] = node2;
        capValue[name] = value;
    }


    void deleteCapacitor(const string& name)
    {
        if (!capNode1.count(name))
            throw runtime_error("Error: Cannot delete capacitor; component not found");

        capNode1.erase(name);
        capNode2.erase(name);
        capValue.erase(name);
    }

    void printAll() const
    {
        for (const auto& c : components)
        {
            const Resistor* r = dynamic_cast<const Resistor*>(c);
            if (r) r->print();
        }
        for (const auto& [name, node1] : capNode1)
        {
            cout << "Capacitor " << name << " between " << node1 << " and " << capNode2.at(name) << " with value " << capValue.at(name) << endl;
        }
        cout << "--- Nodes ---" << endl;
        for (const auto& n : nodes)
            n->print();
    }
};

void handler(Circuit& circuit, string& input)
{
    smatch match;
    regex addResRegex(R"(^add\s+([Rr][A-Za-z0-9_]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_:]+)\s+([0-9.eE+-]+[kK]?|[0-9.eE+-]+(Meg|M)?)$)");
    if (regex_match(input, match, addResRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        string node1 = match[2].str();
        string node2 = match[3].str();
        string value = match[4].str();
        circuit.addResistor(name, node1, node2, value);
        cout << "Resistor " << name << " added successfully.\n";
        return;
    }
    regex delResRegex(R"(^delete\s+([Rr][A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delResRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        circuit.deleteResistor(name);
        cout << "Resistor " << name << " deleted successfully." << endl;
        return;
    }
    regex addCapRegex(R"(^add\s+([Cc][A-Za-z0-9_]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_:]+)\s+([0-9.eE+-]+[uUnnFf]?)$)");
    if (regex_match(input, match, addCapRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        string node1 = match[2].str();
        string node2 = match[3].str();
        string value = match[4].str();
        circuit.addCapacitor(name, node1, node2, value);
        cout << "Capacitor " << name << " added successfully.\n";
        return;
    }
    regex delCapRegex(R"(^delete\s+([Cc][A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delCapRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        circuit.deleteCapacitor(name);
        cout << "Capacitor " << name << " deleted successfully.\n";
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
    cout << endl << "Final circuit:" << endl;
    circuit.printAll();
    return 0;
}
