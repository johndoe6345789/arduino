# Test type annotation parsing in TypthonMini

# Basic type annotations
x: int = 42
name: str = "Hello"
flag: bool = True

print("x:", x)
print("name:", name)
print("flag:", flag)

# List type annotation
numbers: list[int] = [1, 2, 3]
print("numbers:", numbers)

# Dict type annotation
scores: dict[str, int] = {"Alice": 95, "Bob": 87}
print("scores:", scores)

# Type error test (uncomment to test)
# y: int = "wrong type"  # Should produce type error
