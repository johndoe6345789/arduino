# Type Annotation Implementation for TypthonMini

## Overview
This document describes the implementation of type annotation parsing for variables in TypthonMini, enabling Python-style type hints like `x: int = 42`.

## Changes Made

### 1. Header File (TypthonMini.h)

#### Added Type System
```cpp
enum class TypeKind {
    Int,
    Str,
    Bool,
    List,
    Dict,
    Function,
    None,
    Any
};

struct Type {
    TypeKind kind = TypeKind::None;
    std::shared_ptr<Type> elementType;  // for list[T]
    std::shared_ptr<Type> keyType;      // for dict[K,V]
    std::shared_ptr<Type> valueType;    // for dict[K,V]
    std::vector<std::shared_ptr<Type>> paramTypes;  // for function params
    std::shared_ptr<Type> returnType;   // for function return

    static std::shared_ptr<Type> makeInt();
    static std::shared_ptr<Type> makeStr();
    static std::shared_ptr<Type> makeBool();
    static std::shared_ptr<Type> makeNone();
    static std::shared_ptr<Type> makeList(std::shared_ptr<Type> elemType);
    static std::shared_ptr<Type> makeDict(std::shared_ptr<Type> kType, std::shared_ptr<Type> vType);
    static std::shared_ptr<Type> makeFunction(std::vector<std::shared_ptr<Type>> params, std::shared_ptr<Type> ret);

    bool matches(const std::shared_ptr<Type> &other) const;
    std::string toString() const;
};
```

#### Updated Environment Structure
Added `types` map to track variable type annotations:
```cpp
struct Environment {
    std::map<std::string, Value> values;
    std::map<std::string, std::shared_ptr<Type>> types;  // NEW
    std::shared_ptr<Environment> parent;
    // ... rest of fields
};
```

#### Added parseType() Method
```cpp
std::shared_ptr<Type> parseType();
```

### 2. Implementation File (TypthonMini.cpp)

#### Type Factory Methods
Implemented factory methods for creating type objects:
- `Type::makeInt()` - Creates int type
- `Type::makeStr()` - Creates str type
- `Type::makeBool()` - Creates bool type
- `Type::makeList(elemType)` - Creates list[T] type
- `Type::makeDict(keyType, valueType)` - Creates dict[K,V] type

#### Type Utility Methods
- `Type::toString()` - Converts type to string representation
- `Type::matches()` - Checks if two types are compatible

#### parseType() Implementation
Parses type annotations from source code:
```cpp
std::shared_ptr<Type> Interpreter::parseType() {
    Token t = consume();
    if (t.type != Token::Type::Identifier) {
        printError("Expected type name");
        return Type::makeNone();
    }

    if (t.text == "int") {
        return Type::makeInt();
    } else if (t.text == "str") {
        return Type::makeStr();
    } else if (t.text == "bool") {
        return Type::makeBool();
    } else if (t.text == "list") {
        // Parse list[ElementType]
        if (match(Token::Type::Symbol, "[")) {
            auto elemType = parseType();
            if (!match(Token::Type::Symbol, "]")) {
                printError("Expected ] after list element type");
            }
            return Type::makeList(elemType);
        }
        return Type::makeList(Type::makeNone());
    } else if (t.text == "dict") {
        // Parse dict[KeyType, ValueType]
        if (match(Token::Type::Symbol, "[")) {
            auto keyType = parseType();
            if (!match(Token::Type::Symbol, ",")) {
                printError("Expected , in dict type");
            }
            auto valType = parseType();
            if (!match(Token::Type::Symbol, "]")) {
                printError("Expected ] after dict value type");
            }
            return Type::makeDict(keyType, valType);
        }
        return Type::makeDict(Type::makeStr(), Type::makeNone());
    }

    printError("Unknown type");
    return Type::makeNone();
}
```

#### TypedAssignmentStatement Class
New statement type for handling typed variable assignments:
```cpp
struct TypedAssignmentStatement : public Statement {
    std::shared_ptr<Expression> target;
    std::shared_ptr<Type> declaredType;
    std::shared_ptr<Expression> value;

    ExecutionResult execute(Interpreter &interp, std::shared_ptr<Environment> env) override {
        Value rhs = value->evaluate(interp, env);

        // Type check the value
        if (!typeMatches(rhs, declaredType)) {
            Serial.print(F("[TypthonMini] Type error: expected "));
            Serial.print(declaredType->toString().c_str());
            Serial.print(F(", got "));
            Serial.println(valueKindToString(rhs.kind));
            return {};
        }

        if (target->kind == ExpressionKind::Variable) {
            auto var = std::static_pointer_cast<VariableExpression>(target);
            env->define(var->name, rhs);
            env->types[var->name] = declaredType;  // Store type annotation
        }
        return {};
    }
};
```

#### Updated parseAssignmentOrExpr()
Modified to detect and handle type annotations (`:` symbol):
```cpp
std::shared_ptr<Statement> Interpreter::parseAssignmentOrExpr() {
    auto expr = parseExpression();
    Token t = peek();

    // Check for type annotation: name: type = value
    if (t.type == Token::Type::Symbol && t.text == ":") {
        if (expr->kind != ExpressionKind::Variable) {
            printError("Type annotations only allowed on simple variables");
            return std::make_shared<ExpressionStatement>(expr);
        }

        consume(); // consume ':'
        auto typeAnnotation = parseType();

        if (!match(Token::Type::Operator, "=")) {
            printError("Expected = after type annotation");
        }
        auto valueExpr = parseExpression();
        match(Token::Type::Newline);

        return std::make_shared<TypedAssignmentStatement>(expr, typeAnnotation, valueExpr);
    }

    // ... rest of method for regular assignments
}
```

## Supported Syntax

### Basic Type Annotations
```python
x: int = 42
name: str = "Hello"
flag: bool = True
```

### Generic Type Annotations
```python
# List with element type
numbers: list[int] = [1, 2, 3]

# Dictionary with key and value types
scores: dict[str, int] = {"Alice": 95, "Bob": 87}
```

## Type Checking

The implementation performs runtime type checking:
- When a typed assignment is executed, the value's type is checked against the declared type
- If types don't match, an error message is printed to Serial
- Type information is stored in `Environment::types` for future reference

### Type Matching Rules
- `int` matches `ValueKind::Number`
- `str` matches `ValueKind::Text`
- `bool` matches `ValueKind::Boolean`
- `list` matches `ValueKind::List`
- `dict` matches `ValueKind::Dict`

## Example Usage

```python
# Valid typed assignments
x: int = 42
name: str = "Typthon"
numbers: list[int] = [1, 2, 3]
scores: dict[str, int] = {"Alice": 95}

# Type error (will print error message)
y: int = "wrong type"  # Error: expected int, got str
```

## Future Enhancements

Potential areas for extension:
1. Function parameter and return type annotations
2. Class member type annotations
3. Type inference for untyped variables
4. Union types (e.g., `int | str`)
5. Optional types (e.g., `int | None`)
6. Custom class types

## Files Modified

1. `/Users/rmac/Documents/Arduino/libraries/TypthonMini/src/TypthonMini.h`
   - Added TypeKind enum
   - Added Type struct
   - Updated Environment with types map
   - Added parseType() declaration

2. `/Users/rmac/Documents/Arduino/libraries/TypthonMini/src/TypthonMini.cpp`
   - Implemented Type factory methods
   - Implemented parseType()
   - Added TypedAssignmentStatement
   - Modified parseAssignmentOrExpr()

## Testing

A test script has been created at:
`/Users/rmac/Documents/GitHub/arduino/Arduino/TypthonDemo/test_type_annotations.py`

The existing demo in `TypthonDemo.ino` already uses type annotations and should now properly parse and type-check them.
