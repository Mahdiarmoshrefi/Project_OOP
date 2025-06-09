#include <bits/stdc++.h>
#include <filesystem>
namespace fs = std::filesystem;
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

double parseVoltage(const string& s) {
    return stod(s);
}

double parseFrequency(const string& s) {
    return stod(s);
}

double parsePhase(const string& s) {
    return stod(s);
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

class SineVoltageSource : public Component {
    double Voffset;
    double Vamplitude;
    double frequency;

public:
    SineVoltageSource(const string& id, const string& n1, const string& n2,
                      double offset, double amplitude, double freq)
            : Component("SineVoltageSource", id, n1, n2),
              Voffset(offset), Vamplitude(amplitude), frequency(freq) {}

    string getName() const override
    {
        return id;
    }

    void print() const override
    {
        cout << "Sine Voltage Source " << id << " between " << node1 << " and " << node2
             << " with Voffset: " << Voffset << " V, Amplitude: " << Vamplitude
             << " V, Frequency: " << frequency << " Hz" << endl;
    }

    void debug() const override
    {
        Component::debug();
        cout << "[DEBUG] SIN source: offset = " << Voffset
             << ", amplitude = " << Vamplitude
             << ", frequency = " << frequency << endl;
    }

    double valueAt(double t) const
    {
        return Voffset + Vamplitude * sin(2 * M_PI * frequency * t);
    }
};
class VCVS : public Component {
    string ctrlNode1, ctrlNode2;
    double gain;

public:
    VCVS(const string& id, const string& n1, const string& n2, const string& c1, const string& c2, double g)
            : Component("VCVS", id, n1, n2), ctrlNode1(c1), ctrlNode2(c2), gain(g) {}

    string getName() const override { return "E" + id; }

    void print() const override {
        cout << "VCVS " << id << ": " << node1 << " -> " << node2
             << ", controlled by " << ctrlNode1 << " - " << ctrlNode2
             << ", gain = " << gain << endl;
    }
};

class VCCS : public Component {
    string ctrlNode1, ctrlNode2;
    double gain;

public:
    VCCS(const string& id, const string& n1, const string& n2, const string& c1, const string& c2, double g)
            : Component("VCCS", id, n1, n2), ctrlNode1(c1), ctrlNode2(c2), gain(g) {}

    string getName() const override { return "G" + id; }

    void print() const override {
        cout << "VCCS " << id << ": " << node1 << " -> " << node2
             << ", controlled by " << ctrlNode1 << " - " << ctrlNode2
             << ", gain = " << gain << endl;
    }
};

class CCVS : public Component {
    string vname;
    double gain;

public:
    CCVS(const string& id, const string& n1, const string& n2, const string& vname, double g)
            : Component("CCVS", id, n1, n2), vname(vname), gain(g) {}

    string getName() const override { return "H" + id; }

    void print() const override {
        cout << "CCVS " << id << ": " << node1 << " -> " << node2
             << ", controlled by current through " << vname
             << ", gain = " << gain << endl;
    }
};

class CCCS : public Component{
    string vname;
    double gain;

public:
    CCCS(const string& id, const string& n1, const string& n2, const string& vname, double g)
            : Component("CCCS", id, n1, n2), vname(vname), gain(g) {}

    string getName() const override { return "F" + id; }

    void print() const override {
        cout << "CCCS " << id << ": " << node1 << " -> " << node2
             << ", controlled by current through " << vname
             << ", gain = " << gain << endl;
    }
};

class Node{
    string id;
    bool grounded = false;

public:
    Node(const string& id) : id(id) {}

    void setID(const string& newID) { id = newID; }
    string getID() const { return id; }

    void setGrounded(bool g) { grounded = g; }
    bool isGrounded() const { return grounded; }
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
    vector<Node*>& getNodes() { return nodes; }
    vector<Component*>& getComponents() { return components; }
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

    void addSineVoltage(const string& name, const string& node1, const string& node2, double offset, double amplitude, double frequency)
    {

        for (const auto& c : components)
            if (c->getName() == name)
                throw DuplicateElementI(name);

        ensureNodeExists(node1);
        ensureNodeExists(node2);

        components.push_back(new SineVoltageSource(name, node1, node2, offset, amplitude, frequency));
    }

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
        Node *node = getOrCreateNode(name);
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

    vector<string> getAllNodeNames() const
    {
        vector<string> names;
        for (const Node* node : nodes)
        {
            names.push_back(node->getID());
        }
        return names;
    }


    void printAllComponents() const
    {
        for (const Component* comp : components)
        {
            cout << comp->getInfo() << endl;
        }
    }

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

    void addNode(const string& nodeName)
    {
        for (auto node : nodes)
        {
            if (node->getID() == nodeName)
                return;
        }
        nodes.push_back(new Node(nodeName));
        if (nodeName == "GND")
            gndExists = true;
    }

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
            else if (auto V = dynamic_cast<const VoltageSource*>(c))
                V->print();
            else if (auto C = dynamic_cast<const CurrentSource*>(c))
                C->print();
            else if (auto s = dynamic_cast<const SineVoltageSource*>(c))
                s->print();
            else if (auto vcvs = dynamic_cast<const VCVS*>(c))
                vcvs->print();
            else if (auto vccs = dynamic_cast<const VCCS*>(c))
                vccs->print();
            else if (auto ccvs = dynamic_cast<const CCVS*>(c))
                ccvs->print();
            else if (auto cccs = dynamic_cast<const CCCS*>(c))
                cccs->print();
            else
                cout << "Unknown component type." << endl;
        }
    }

    void addComponent(Component* c)
    {
        components.push_back(c);
        cout << "[ADDED] " << c->getInfo() << endl;
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

void showExistingSchematics() {
    string folderPath = "./schematics";
    vector<string> filenames;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            string name = entry.path().filename().string();
            filenames.push_back(name);
        }
    }

    if (filenames.empty()) {
        cout << "No schematics found in the 'schematics' folder.\n";
        return;
    }

    while (true) {
        cout << "-choose existing schematic:\n";
        for (int i = 0; i < filenames.size(); ++i) {
            string name = filenames[i];
            size_t dot = name.find_last_of('.');
            if (dot != string::npos) name = name.substr(0, dot);
            cout << i + 1 << "-" << name << '\n';
        }

        cout << "Type a number to view schematic, or 'return' to go back:\n";
        string input;
        getline(cin, input);

        if (input == "return") break;

        bool isValid = true;
        for (char c : input) {
            if (!isdigit(c)) {
                isValid = false;
                break;
            }
        }

        if (!isValid) {
            cout << "-Error : Inappropriate input\n";
            continue;
        }

        int idx = stoi(input);
        if (idx < 1 || idx > filenames.size()) {
            cout << "-Error : Inappropriate input\n";
            continue;
        }

        string fullPath = folderPath + "/" + filenames[idx - 1];
        ifstream file(fullPath);
        if (!file.is_open()) {
            cout << "Failed to open file: " << fullPath << '\n';
            continue;
        }

        cout << "\nContents of " << filenames[idx - 1] << ":\n";
        string line;
        while (getline(file, line)) {
            cout << line << '\n';
        }
        file.close();

        cout << "\n---------------------------\n\n";
    }
}
void handler(Circuit& circuit, string& input)
{
    smatch match;
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
        vector<string> nodeList = circuit.getAllNodeNames();
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
    regex vcvsRegex(R"(^add\s+E([A-Za-z0-9_]+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+)$)");
    if (regex_match(input, match, vcvsRegex)) {
        string id = match[1], n1 = match[2], n2 = match[3], c1 = match[4], c2 = match[5];
        double gain = stod(match[6]);
        circuit.addComponent(new VCVS(id, n1, n2, c1, c2, gain));
        return;
    }


    regex vccsRegex(R"(^add\s+G([A-Za-z0-9_]+)\s+(\w+)\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+)$)");
    if (regex_match(input, match, vccsRegex)) {
        string id = match[1], n1 = match[2], n2 = match[3], c1 = match[4], c2 = match[5];
        double gain = stod(match[6]);
        circuit.addComponent(new VCCS(id, n1, n2, c1, c2, gain));
        return;
    }

    regex ccvsRegex(R"(^add\s+H([A-Za-z0-9_]+)\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+)$)");
    if (regex_match(input, match, ccvsRegex)) {
        string id = match[1], n1 = match[2], n2 = match[3], vname = match[4];
        double gain = stod(match[5]);
        circuit.addComponent(new CCVS(id, n1, n2, vname, gain));
        return;
    }

    regex cccsRegex(R"(^add\s+F([A-Za-z0-9_]+)\s+(\w+)\s+(\w+)\s+(\w+)\s+([+-]?\d*\.?\d+)$)");
    if (regex_match(input, match, cccsRegex)) {
        string id = match[1], n1 = match[2], n2 = match[3], vname = match[4];
        double gain = stod(match[5]);
        circuit.addComponent(new CCCS(id, n1, n2, vname, gain));
        return;
    }
    throw SyntaxError();
}
void validateCircuit(Circuit& circuit)
{
    auto& nodes = circuit.getNodes();
    auto& components = circuit.getComponents();

    int groundCount = 0;
    for (auto node : nodes)
    {
        if (node->isGrounded())
            groundCount++;
    }

    if (groundCount == 0)
        throw runtime_error("Error: No ground node detected in the circuit.");
    if (groundCount > 1)
        throw runtime_error("Error: More than one ground node detected in the circuit.");

    set<string> nodeNames;
    for (auto node : nodes)
    {
        if (nodeNames.find(node->getID()) != nodeNames.end())
            throw runtime_error("Error: Duplicate node name detected: " + node->getID());
        nodeNames.insert(node->getID());
    }

    set<string> compNames;
    for (auto comp : components)
    {
        if (compNames.find(comp->getID()) != compNames.end())
            throw runtime_error("Error: Duplicate component name detected: " + comp->getID());
        compNames.insert(comp->getID());
    }
}

vector<string> schematics = {
        "schematics/draft1.txt",
        "schematics/draft2.txt",
        "schematics/draft3.txt",
        "schematics/elecphase1.txt"
};
bool isNumber(const string& s) {
    for (char c : s) if (!isdigit(c)) return false;
    return !s.empty();
}

string convertToCommand(const string& line) {
    istringstream iss(line);
    string first;
    iss >> first;

    if (first.empty()) return line;

    char c = toupper(first[0]);
    if (c == 'R' || c == 'C' || c == 'L' || c == 'V' || c == 'I' || c == 'D') {
        return "add " + line;
    }

    return line;
}
void loadSchematicFromFile(const string& filepath, Circuit& currentCircuit)
{
    ifstream file(filepath);
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file: " << filepath << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        string converted = convertToCommand(line);
        try {
            handler(currentCircuit, converted);
        } catch (const exception& ex) {
            cout << "[Exception in file] " << ex.what() << endl;
        }
    }

    file.close();
}

int main()
{
    vector<Circuit> circuits;
    vector<bool> circuitValidity;
    Circuit currentCircuit;
    string line;

    vector<string> schematicFiles;

    for (const auto& entry : fs::directory_iterator("./pictures/schematics")) {
        if (entry.is_regular_file())
            schematicFiles.push_back(entry.path().filename().string());
    }

    cout << "Enter commands (type 'exit' to quit):\n";
    while (true)
    {
        cout << "> ";
        getline(cin, line);

        if (line == "exit") {
            circuits.push_back(currentCircuit);
            try {
                validateCircuit(currentCircuit);
                circuitValidity.push_back(true);
            } catch (const exception& ex) {
                cout << "[ERROR] Final circuit is invalid:\n" << ex.what() << endl;
                circuitValidity.push_back(false);
            }
            break;
        }
        else if (line == "another circuit") {
            circuits.push_back(currentCircuit);
            try {
                validateCircuit(currentCircuit);
                circuitValidity.push_back(true);
            } catch (const exception& ex) {
                cout << "[ERROR] Cannot start a new circuit:\n" << ex.what() << endl;
                circuitValidity.push_back(false);
            }
            currentCircuit = Circuit();
            cout << "Switched to a new circuit.\n";
            continue;
        }
        else if (line == "show existing schematics") {
            showExistingSchematics();
            continue;
        }
        try {
            handler(currentCircuit, line);
        } catch (const exception& ex) {
            cout << "[Exception] " << ex.what() << endl;
        }
    }

//    cout << "\n All Circuits Summary:\n";
//    for (int i = 0; i < circuits.size(); ++i)
//    {
//        cout << "-----------------------------\n";
//        cout << "Circuit " << i + 1 << ":\n";
//        if (!circuitValidity[i])
//            cout << "[!] This circuit is INVALID.\n";
//        circuits[i].printAll();
//    }

    return 0;
}




