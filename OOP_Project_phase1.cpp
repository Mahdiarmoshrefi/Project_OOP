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

class Component {
protected:
    string type;
    string id;
    string node1, node2;

public:
    Component(const string& t, const string& id, const string& n1, const string& n2)
            : type(t), id(id), node1(n1), node2(n2) {}

    virtual string getName() const = 0;
    virtual ~Component() = default;

    const string& getType() const { return type; }
    const string& getID() const { return id; }
    const string& getNode1() const { return node1; }
    const string& getNode2() const { return node2; }

    string getInfo() const {
        return type + " " + id + " between " + node1 + " and " + node2;
    }

    virtual void print() const = 0;

    virtual void debug() const {
        cout << "[DEBUG] " << getInfo() << endl;
    }
};


class Resistor : public Component {
    double resistanceValue;
public:
    Resistor(const string& id, const string& n1, const string& n2, double val)
            : Component("Resistor", id, n1, n2), resistanceValue(val) {}

    string getName() const override { return id; }

    void print() const override {
        cout << "Resistor " << id << " between " << node1 << " and " << node2
             << " with value " << resistanceValue << " Ohms" << endl;
    }
};


class Capacitor : public Component {
    double capacitanceValue;
public:
    Capacitor(const string& id, const string& n1, const string& n2, double val)
            : Component("Capacitor", id, n1, n2), capacitanceValue(val) {}

    string getName() const override { return id; }

    void print() const override {
        cout << "Capacitor " << id << " between " << node1 << " and " << node2
             << " with value " << capacitanceValue << " F" << endl;
    }
};


class Inductor : public Component {
    double inductanceValue;
public:
    Inductor(const string& id, const string& n1, const string& n2, double val)
            : Component("Inductor", id, n1, n2), inductanceValue(val) {}

    string getName() const override { return id; }

    void print() const override {
        cout << "Inductor " << id << " between " << node1 << " and " << node2
             << " with value " << inductanceValue << " H" << endl;
    }
};

class Diode : public Component {
    string model;
    double threshold;
public:
    Diode(const string& id, const string& n1, const string& n2, const string& m)
            : Component("Diode", id, n1, n2), model(m) {
        if (model == "D")
            threshold = 0.0;
        else if (model == "Z")
            threshold = 0.7;
        else
            throw runtime_error("Error: Model " + model + " not found in library");
    }

    string getName() const override { return id; }

    void print() const override {
        cout << "Diode " << id << " between " << node1 << " and " << node2
             << " with model " << model << " (threshold: " << threshold << " V)" << endl;
    }
};

class VoltageSource : public Component {
public:
    double value;

    VoltageSource(const string& id, const string& n1, const string& n2, double val)
            : Component("VoltageSource", id, n1, n2), value(val) {}

    string getName() const override {
        return id;
    }

    void print() const override {
        cout << "Voltage Source " << id << " between " << node1 << " and " << node2
             << " with value " << value << " V" << endl;
    }

    void debug() const override {
        Component::debug();
        cout << "[DEBUG] Voltage value: " << value << " V" << endl;
    }
};


class CurrentSource : public Component {
public:
    double value;

    CurrentSource(const string& id, const string& n1, const string& n2, double val)
            : Component("CurrentSource", id, n1, n2), value(val) {}

    string getName() const override {
        return id;
    }

    void print() const override {
        cout << "Current Source " << id << " between " << node1 << " and " << node2
             << " with value " << value << " A" << endl;
    }

    void debug() const override {
        Component::debug();
        cout << "[DEBUG] Current value: " << value << " A" << endl;
    }
};



class Node {
    string id;
    bool grounded;
public:
    Node(const string& id) : id(id), grounded(false) {}

    const string& getID() const {
        return id;
    }

    void setID(const string& newID) {
        id = newID;
    }

    void setGrounded(bool g) {
        grounded = g;
    }

    bool isGrounded() const {
        return grounded;
    }
};

class Circuit
{
    vector<Component*> components;

    map<string, string> capNode1;
    map<string, string> capNode2;
    map<string, string> capValue;

    vector<Node*> nodes;

    // این تابع چک می‌کنه نود با آیدی داده شده وجود داره یا نه، اگر نبود ایجادش می‌کنه
    void ensureNodeExists(const string& id)
    {
        for (auto node : nodes)
            if (node->getID() == id)
                return;
        nodes.push_back(new Node(id));
    }

public:
    // دکتر
    ~Circuit()
    {
        for (auto c : components)
            delete c;
        for (auto n : nodes)
            delete n;
    }
    void addVoltageSource(const string& name, const string& node1, const string& node2, double value)
    {
        components.push_back(new VoltageSource(name, node1, node2, value));
        cout << "Voltage Source " << name << " added.\n";
    }

    void addCurrentSource(const string& name, const string& node1, const string& node2, double value)
    {
        components.push_back(new CurrentSource(name, node1, node2, value));
        cout << "Current Source " << name << " added.\n";
    }
    // اضافه کردن مقاومت
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

    // حذف مقاومت
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

    // اضافه کردن خازن
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

    // حذف خازن
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

    // اضافه کردن سلف
    void addInductor(string& name, string& node1, string& node2, string& valueStr)
    {
        if (name.empty() || name[0] != 'L')
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

    // حذف سلف
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

    // اضافه کردن دیود
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

    // حذف دیود
    void deleteDiode(const string& name)
    {
        for (int i = 0; i < (int)components.size(); ++i)
        {
            if (components[i]->getName() == name)
            {
                if (dynamic_cast<Diode*>(components[i]))
                {
                    delete components[i];
                    components.erase(components.begin() + i);
                    return;
                }
            }
        }
        throw runtime_error("Error: Cannot delete diode; component not found");
    }

    // گرفتن یا ایجاد نود
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

    // اضافه کردن گره زمین
    void addGround(const string& name)
    {
        Node* node = getOrCreateNode(name);
        node->setGrounded(true);
        cout << "Ground connected to node " << name << " successfully." << endl;
    }

    // حذف زمین از گره
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

    // گرفتن نام همه نودها
    vector<string> getAllNodeNames() const
    {
        vector<string> names;
        for (const Node* node : nodes)
        {
            names.push_back(node->getID());
        }
        return names;
    }

    // چاپ همه کامپوننت‌ها
    void printAllComponents() const
    {
        for (const Component* comp : components)
        {
            cout << comp->getInfo() << endl;
        }
    }

    // چاپ کامپوننت‌های یک نوع خاص
    void printComponentsOfType(const string& type) const
    {
        bool found = false;
        for (const Component* comp : components)
        {
            if (comp->getType() == type)
            {
                cout << comp->getInfo() << endl;
                found = true;
            }
        }
        if (!found)
            cout << "No components of type " << type << " found." << endl;
    }

    // تغییر نام یک نود
    void renameNode(const string& oldName, const string& newName)
    {
        if (!nodeExists(oldName))
            throw runtime_error("ERROR: Node " + oldName + " does not exist in the circuit");
        if (nodeExists(newName))
            throw runtime_error("ERROR: Node name " + newName + " already exists");

        for (Node* node : nodes)
        {
            if (node->getID() == oldName)
            {
                node->setID(newName);
                return;
            }
        }
    }

    // چک کردن وجود نود
    bool nodeExists(const string& name) const
    {
        for (const Node* node : nodes)
        {
            if (node->getID() == name)
                return true;
        }
        return false;
    }

    bool gndExists = false;

    // اضافه کردن نود (اصلاح شده)
    void addNode(const string& nodeName)
    {
        for (auto node : nodes)
        {
            if (node->getID() == nodeName)
                return; // اگر هست اضافه نکن
        }
        nodes.push_back(new Node(nodeName));
        if (nodeName == "GND")
            gndExists = true;
    }

    // چک کردن وجود GND
    bool hasGND() const
    {
        return gndExists;
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
    if (input == ".nodes")
    {
        vector<string> nodeList = circuit.getAllNodeNames(); // NOT const anymore
        if (circuit.hasGND() && find(nodeList.begin(), nodeList.end(), "GND") == nodeList.end())
        {
            nodeList.push_back("GND");
        }

        if (nodeList.empty())
        {
            cout << "No nodes in the circuit." << endl;
        }
        else
        {
            cout << "Available nodes:\n";
            for (size_t i = 0; i < nodeList.size(); ++i)
            {
                cout << nodeList[i];
                if (i < nodeList.size() - 1)
                    cout << ", ";
            }
            cout << endl;
        }
        return;
    }

    regex listRegex(R"(^\.list(?:\s+([A-Za-z0-9_]+))?$)");
    if (regex_match(input, match, listRegex))
    {
        if (match[1].matched)
        {
            string type = match[1].str();
            circuit.printComponentsOfType(type);
        }
        else
        {
            circuit.printAllComponents();
        }
        return;
    }
    regex renameNodeRegex(R"(^\.rename\s+node\s+([A-Za-z0-9_]+)\s+([A-Za-z0-9_]+)$)");
    if (regex_match(input, match, renameNodeRegex))
    {
        string oldName = match[1].str();
        string newName = match[2].str();
        if (!isValVertexID(oldName) || !isValVertexID(newName))
            throw SyntaxError("ERROR: Invalid syntax - correct format:\n.rename node <old_name> <new_name>");
        try
        {
            circuit.renameNode(oldName, newName);
            cout << "SUCCESS: Node renamed from " << oldName << " to " << newName << endl;
        }
        catch (const runtime_error& e)
        {
            cerr << e.what() << endl;
        }
        return;
    }
    regex voltageRegex(R"(^add\s+VoltageSource\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?))");
    regex currentRegex(R"(^add\s+CurrentSource\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+(?:[eE][+-]?\d+)?))");

    if (regex_match(input, match, voltageRegex))
    {
        string name = match[1];
        string n1 = match[2];
        string n2 = match[3];
        double value = stod(match[4]);
        circuit.addVoltageSource(name, n1, n2, value);
        return;
    }
    else if (regex_match(input, match, currentRegex))
    {
        string name = match[1];
        string n1 = match[2];
        string n2 = match[3];
        double value = stod(match[4]);
        circuit.addCurrentSource(name, n1, n2, value);
        return;
    }
    else if (input.find("add VoltageSource") == 0) {
        cout << "ERROR: Invalid syntax - correct format:\nadd VoltageSource <Name> <Node1> <Node2> <Value>\n";
    }
    else if (input.find("add CurrentSource") == 0) {
        cout << "ERROR: Invalid syntax - correct format:\nadd CurrentSource <Name> <Node1> <Node2> <Value>\n";
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
