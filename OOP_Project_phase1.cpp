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

class DuplicateElementR : public exception
{
    string msg;
public:
    explicit DuplicateElementR(const string& name)
    {
        msg = "Error: Resistor " + name + " already exists in the circuit";
    }
    const char* what() const noexcept override
    {
        return msg.c_str();
    }
};

class DuplicateElementC : public exception
{
    string msg;
public:
    explicit DuplicateElementC(const string& name)
    {
        msg = "Error: Capacitor " + name + " already exists in the circuit";
    }
    const char* what() const noexcept override
    {
        return msg.c_str();
    }
};
class DuplicateElementI : public exception
{
    string msg;
public:
    explicit DuplicateElementI(const string& name)
    {
        msg = "Error: Inductor " + name + " already exists in the circuit";
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

double parseInductance(const string& s)
{
    try {
        size_t idx;
        double val = stod(s, &idx);
        string unit = s.substr(idx);
        if (unit == "m")
            val *= 1e-3;
        else if (unit == "u" || unit == "µ")
            val *= 1e-6;
        else if (unit == "n")
            val *= 1e-9;
        else if (unit.empty())
            val *= 1;
        else if (unit.find('e') != string::npos || unit.find('E') != string::npos)
            val = stod(s);
        else throw SyntaxError("Error: Invalid unit for inductance");
        return val;
    }
    catch (...)
    {
        throw SyntaxError("Error: Invalid value for inductance");
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

class Capacitor : public Component
{
    string name, node1, node2;
    double capacitanceValue;
public:
    Capacitor(string n, string n1, string n2, double val) : name(move(n)), node1(move(n1)), node2(move(n2)), capacitanceValue(val) {}
    string getName() const override { return name; }
    void print() const
    {
        cout << "Capacitor " << name << " between " << node1 << " and " << node2 << " with value " << capacitanceValue << " F" << endl;
    }
};

class Inductor : public Component
{
    string name, node1, node2;
    double inductanceValue;
public:
    Inductor(string n, string n1, string n2, double val) : name(move(n)), node1(move(n1)), node2(move(n2)), inductanceValue(val) {}
    string getName() const override { return name; }
    void print() const {
        cout << "Inductor " << name << " between " << node1 << " and " << node2 << " with value " << inductanceValue << " H" << endl;
    }
};

class Diode : public Component
{
    string name, node1, node2, model;
    double threshold;
public:
    Diode(string n, string n1, string n2, string m) : name(move(n)), node1(move(n1)), node2(move(n2)), model(move(m))
    {
        if (model == "D")
            threshold = 0.0;
        else if (model == "Z")
            threshold = 0.7;
        else
            throw runtime_error("Error: Model " + model + " not found in library");
    }
    string getName() const override
    {
        return name;
    }
    void print() const
    {
        cout << "Diode " << name << " between " << node1 << " and " << node2
             << " with model " << model << " (threshold: " << threshold << " V)" << endl;
    }
};

class Node
{
    string id;
    double voltage;
    bool grounded = false;
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

    void print() const
    {
        cout << "Node " << id << " with voltage: " << voltage << " V" << endl;
    }
    bool isGrounded() const
    {
        return grounded;
    }

    void setGrounded(bool g)
    {
        grounded = g;
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
                throw DuplicateElementR(name);
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

    void addCapacitor(string& name, string& node1, string& node2, string& valueStr)
    {
        if (name.empty() || name[0] != 'C')
            throw ElementNotFound(name);

        for (const auto& c : components)
            if (c->getName() == name)
                throw DuplicateElementC(name);

        double value = parseCapValue(valueStr);
        if (value <= 0)
            throw SyntaxError("Error: Capacitance cannot be zero or negative");

        ensureNodeExists(node1);
        ensureNodeExists(node2);

        components.push_back(new Capacitor(name, node1, node2, value));
    }
    void deleteCapacitor(const string& name)
    {
        for (int i = 0; i < (int)components.size(); ++i)
        {
            if (components[i]->getName() == name)
            {
                if (dynamic_cast<Capacitor*>(components[i]))
                {
                    delete components[i];
                    components.erase(components.begin() + i);
                    return;
                }
            }
        }
        throw runtime_error("Error: Cannot delete capacitor; component not found");
    }
    void addInductor(string& name, string& node1, string& node2, string& valueStr)
    {
        if (name[0] != 'L')
            throw runtime_error("Error: Element " + name + " not found in library");

        for (const auto& c : components)
            if (c->getName() == name)
                throw DuplicateElementI(name);

        double value = parseInductance(valueStr);
        if (value <= 0)
            throw SyntaxError("Error: Inductance cannot be zero or negative");

        ensureNodeExists(node1);
        ensureNodeExists(node2);

        components.push_back(new Inductor(name, node1, node2, value));
    }
    void deleteInductor(const string& name)
    {
        for (int i = 0; i < (int)components.size(); ++i)
        {
            if (components[i]->getName() == name)
            {
                if (dynamic_cast<Inductor*>(components[i]))
                {
                    delete components[i];
                    components.erase(components.begin() + i);
                    return;
                }
            }
        }
        throw runtime_error("Error: Cannot delete inductor; component not found");
    }
    void addDiode(const string& name, const string& node1, const string& node2, const string& model)
    {
        if (name.empty() || name[0] != 'D')
            throw ElementNotFound(name);

        for (const auto& c : components)
            if (c->getName() == name)
                throw runtime_error("Error: diode " + name + " already exists in the circuit");

        if (model != "D" && model != "Z")
            throw runtime_error("Error: Model " + model + " not found in library");

        ensureNodeExists(node1);
        ensureNodeExists(node2);

        components.push_back(new Diode(name, node1, node2, model));
    }

    void deleteDiode(const string& name)
    {
        for (int i = 0; i < (int)components.size(); ++i)
        {
            if (components[i]->getName() == name)
            {
                const Diode* d = dynamic_cast<const Diode*>(components[i]);
                if (d)
                {
                    delete components[i];
                    components.erase(components.begin() + i);
                    return;
                }
            }
        }
        throw runtime_error("Error: Cannot delete diode; component not found");
    }

    Node* getOrCreateNode(const string& id)
    {
        for (Node* node : nodes)
        {
            if (node->getID() == id)
                return node;
        }
        Node* newNode = new Node(id);
        nodes.push_back(newNode);
        return newNode;
    }

    void addGround(const string& name)
    {
        Node* node = getOrCreateNode(name);
        node->setGrounded(true);
        cout << "Ground connected to node " << name << " successfully." << endl;
    }

    void deleteGround(const string& nodeName)
    {
        for (Node* node : nodes)
        {
            if (node->getID() == nodeName)
            {
                node->setGrounded(false);
                cout << "Ground removed from node " << nodeName << " successfully." << endl;
                return;
            }
        }
        throw runtime_error("Node does not exist");
    }

    void printAll() const
    {
        for (const auto& c : components)
        {
            if (auto r = dynamic_cast<const Resistor*>(c))
                r->print();
            else if (auto l = dynamic_cast<const Inductor*>(c))
                l->print();
            else if (auto cp = dynamic_cast<const Capacitor*>(c))
                cp->print();
            else if (auto d = dynamic_cast<const Diode*>(c))
                d->print();
        }
    }
};

bool isValVertexID(const string& s)
{
    if (s.empty())
        return false;
    if (!isalnum(s[0]))
        return false;
    for (char c : s)
        if (!isalnum(c) && c != '_')
            return false;
    return true;
}

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
        cout << "Capacitor " << name << " added successfully." << endl;
        return;
    }
    regex delCapRegex(R"(^delete\s+([Cc][A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delCapRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        circuit.deleteCapacitor(name);
        cout << "Capacitor " << name << " deleted successfully." << endl;
        return;
    }
    regex addIndRegex(R"(^add\s+([Ll][A-Za-z0-9_]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_:]+)\s+([0-9.eE+-]+(u|U|µ|m|M|H)?)$)");
    if (regex_match(input, match, addIndRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        string node1 = match[2].str();
        string node2 = match[3].str();
        string value = match[4].str();
        circuit.addInductor(name, node1, node2, value);
        cout << "Inductor " << name << " added successfully." << endl;
        return;
    }
    regex delIndRegex(R"(^delete\s+([Ll][A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delIndRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        circuit.deleteInductor(name);
        cout << "Inductor " << name << " deleted successfully." << endl;
        return;
    }
    regex addDiodeRegex(R"(^add\s+([Dd][A-Za-z0-9_]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_:]+)\s+([A-Za-z0-9_]+)$)");
    if (regex_match(input, match, addDiodeRegex))
    {
        string name = match[1].str();
        string node1 = match[2].str();
        string node2 = match[3].str();
        string model = match[4].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        if (model != "D" && model != "Z")
            throw runtime_error("Error: Model " + model + " not found in library");
        circuit.addDiode(name, node1, node2, model);
        cout << "Diode " << name << " added successfully.\n";
        return;
    }
    regex delDiodeRegex(R"(^delete\s+(D[A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delDiodeRegex))
    {
        string name = match[1].str();
        if (!isupper(name[0]))
            throw ElementNotFound(name);
        circuit.deleteDiode(name);
        cout << "Diode " << name << " deleted successfully." << endl;
        return;
    }
    regex addGNDRegex(R"(^add\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)$)");
    if (regex_match(input, match, addGNDRegex))
    {
        string elementType = match[1].str();
        string node = match[2].str();
        if (elementType != "GND")
        {
            throw runtime_error("Error: Element " + elementType + " not found in library");
        }
        if (!isValVertexID(node))
            throw SyntaxError("Error: Syntax error");
        circuit.addGround(node);
        return;
    }
    regex delGNDRegex(R"(^delete\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)$)");
    if (regex_match(input, match, delGNDRegex))
    {
        string elementType = match[1].str();
        string node = match[2].str();
        if (elementType != "GND")
        {
            throw runtime_error("Error: Element " + elementType + " not found in library");
        }
        if (!isValVertexID(node))
            throw SyntaxError("Error: Syntax error");
        circuit.deleteGround(node);
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
//ground debug