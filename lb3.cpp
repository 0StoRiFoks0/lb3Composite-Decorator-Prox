#include <iostream>
#include <string>
#include <memory>
#include <cctype>

using namespace std;

// --- Composite ---

// Абстрактний клас виразів
class Expression {
public:
    virtual ~Expression() = default;
    virtual void print() const = 0;
};

// Константа
class Constant : public Expression {
    double value;
public:
    Constant(double val) : value(val) {}
    void print() const override {
        cout << value;
    }
};

// Змінна
class Variable : public Expression {
    string name;
public:
    Variable(const string& n) : name(n) {}
    void print() const override {
        cout << name;
    }
};

// Бінарна операція
class BinaryExpression : public Expression {
    char op;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;
public:
    BinaryExpression(char oper, shared_ptr<Expression> l, shared_ptr<Expression> r)
        : op(oper), left(move(l)), right(move(r)) {}

    void print() const override {
        cout << "(";
        left->print();
        cout << " " << op << " ";
        right->print();
        cout << ")";
    }
};

// --- Decorator ---

class ExpressionDecorator : public Expression {
protected:
    shared_ptr<Expression> expr;
public:
    ExpressionDecorator(shared_ptr<Expression> e) : expr(move(e)) {}
    void print() const override {
        expr->print();
    }
};

// Логування
class LoggerDecorator : public ExpressionDecorator {
public:
    LoggerDecorator(shared_ptr<Expression> e) : ExpressionDecorator(move(e)) {}
    void print() const override {
        cout << "[Logger] Вивід виразу: ";
        expr->print();
        cout << " [Кінець логування]" << endl;
    }
};

// --- Proxy ---

class VariableProxy : public Expression {
    string name;
    mutable shared_ptr<Variable> realVariable;
    bool hasAccess;
public:
    VariableProxy(const string& n, bool access = true)
        : name(n), hasAccess(access), realVariable(nullptr) {}

    void print() const override {
        if (!hasAccess) {
            cout << "[Доступ заборонено до змінної " << name << "]";
            return;
        }
        if (!realVariable) {
            realVariable = make_shared<Variable>(name);
        }
        realVariable->print();
    }
};

// --- Допоміжна функція для створення підвиразу ---

shared_ptr<Expression> createOperand(const string& token) {
    // Перевірка чи це число
    if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
        return make_shared<Constant>(stod(token));
    } else {
        char access;
        cout << "Дозволити доступ до змінної '" << token << "'? (y/n): ";
        cin >> access;
        return make_shared<VariableProxy>(token, access == 'y' || access == 'Y');
    }
}

// --- Головна програма ---

int main() {
    cout << "=== Побудова власного виразу у форматі: operand1 operator operand2 ===\n";
    cout << "Приклад: 3 + x  або  y * 7\n\n";

    string leftInput, rightInput;
    char op;

    // Введення користувачем
    cout << "Введи перший операнд: ";
    cin >> leftInput;

    cout << "Введи оператор (+, -, *, /): ";
    cin >> op;

    cout << "Введи другий операнд: ";
    cin >> rightInput;

    auto leftExpr = createOperand(leftInput);
    auto rightExpr = createOperand(rightInput);
    auto fullExpr = make_shared<BinaryExpression>(op, leftExpr, rightExpr);

    // Запит на логування
    char logChoice;
    cout << "\nХочеш вивести вираз з логуванням? (y/n): ";
    cin >> logChoice;

    cout << "\n=== Результат ===\n";
    if (logChoice == 'y' || logChoice == 'Y') {
        auto loggedExpr = make_shared<LoggerDecorator>(fullExpr);
        loggedExpr->print();
    } else {
        fullExpr->print();
        cout << endl;
    }

    cout << "\nНатисни Enter для виходу...";
    cin.ignore();
    cin.get();

    return 0;
}
