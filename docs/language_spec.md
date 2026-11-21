# YaarLang: Official Language Specification
**Course:** CS 4031 — Compiler Construction  
**Institution:** FAST-NUCES (Faisalabad-Chiniot Campus)  
**Authors:** Compiler Project Team  
**Paradigm:** Imperative, Statically Typed, C-Family Bracket Syntax `{}`  
**File Extension:** `.yaar`

---

## 1. Introduction & Overview

`YaarLang` is an expressive, statically typed educational programming language designed for compiler construction. It blends familiar C++ block-scoping (`{}`) and control constructs with natural Pakistani collegiate slang.

Every `YaarLang` program begins with the token pair `sun yaar {` and terminates with `} khatam yaar`.

---

## 2. Lexical Structure & Tokens

### 2.1 Keywords
| Keyword | Category | Usage / Description |
| :--- | :--- | :--- |
| `sun` | Program Boundary | Used with `yaar {` to begin the program |
| `khatam` | Program Boundary | Used with `yaar` to close the program |
| `yaar` | Language Anchor | Delimiter anchor used across control constructs |
| `rakh` | Declaration | Initiates variable declarations (`rakh yaar <type> <id> = <val>;`) |
| `number` | Data Type | 32-bit signed integer (`int`) |
| `float` | Data Type | 64-bit IEEE 754 floating point |
| `char` | Data Type | Single ASCII character enclosed in single quotes |
| `string` | Data Type | Character array enclosed in double quotes |
| `bool` | Data Type | Boolean type accepting `pakka` or `topi` |
| `pakka` | Boolean Literal | Logical `true` |
| `topi` | Boolean Literal | Logical `false` |
| `bol` | I/O | Standard output print statement (`bol yaar(...)`) |
| `puch` | I/O | Standard input read statement (`puch yaar(...)`) |
| `agar` | Conditional | Conditional branch (`agar yaar (...) { ... }`) |
| `warna` | Conditional | Else branch (`warna { ... }` or `warna agar (...) { ... }`) |
| `dekh` | Selection | Switch statement (`dekh yaar (...) { ... }`) |
| `sardi` | Selection Case | Case branch in switch statement (`sardi <val>:`) |
| `garmi` | Selection Default | Default branch in switch statement (`garmi:`) |
| `ghoom` | Iteration | For loop (`ghoom yaar (init; cond; step) { ... }`) |
| `jab` | Iteration | Used in while loop (`jab tak yaar (...) { ... }`) |
| `tak` | Iteration | Sub-token of `jab tak` |
| `chala` | Iteration | Do-While loop (`chala yaar { ... } jab tak yaar (...);`) |
| `bas` | Loop Control | Break statement (`bas kar yaar;`) |
| `kar` | Loop Control | Break sub-token (`bas kar yaar;`) |
| `agay` | Loop Control | Continue statement (`agay chal yaar;`) |
| `chal` | Loop Control | Continue sub-token (`agay chal yaar;`) |
| `jugaad` | Function | Function definition (`jugaad <type> <name>(...) { ... }`) |
| `de` | Return | Return statement (`de yaar <expr>;`) |
| `list` | Data Structure | Fixed-size array declaration (`list yaar <type> <id>[<size>];`) |

---

### 2.2 Operators & Delimiters
- **Arithmetic:** `+`, `-`, `*`, `/`, `%`
- **Relational:** `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Logical:** `&&` (AND), `||` (OR), `!` (NOT)
- **Assignment:** `=`
- **Delimiters:** `(`, `)`, `{`, `}`, `[`, `]`, `;`, `,`, `:`

---

## 3. Mandatory Language Constructs & Syntax Guide

### 3.1 Program Skeleton
```c
sun yaar {
    // Statements and program body
}
khatam yaar
```

### 3.2 Variables and Data Types
```c
rakh yaar number count = 10;
rakh yaar float gpa = 3.65;
rakh yaar char grade = 'A';
rakh yaar string name = "Ali";
rakh yaar bool isPassed = pakka; // pakka (true), topi (false)
```

### 3.3 Input & Output
```c
bol yaar("Enter your age:");
puch yaar(age);
bol yaar("Your age is:");
bol yaar(age);
```

### 3.4 Conditionals (If / Else If / Else)
```c
agar yaar (marks >= 80) {
    bol yaar("Grade: A");
} warna agar (marks >= 50) {
    bol yaar("Grade: Pass");
} warna {
    bol yaar("Grade: Fail");
}
```

### 3.5 Selection Statement (`dekh yaar` with `sardi` & `garmi`)
```c
dekh yaar (op) {
    sardi '+':
        result = a + b;
        bas kar yaar;
    sardi '-':
        result = a - b;
        bas kar yaar;
    sardi '*':
        result = a * b;
        bas kar yaar;
    sardi '/':
        agar yaar (b != 0) {
            result = a / b;
        } warna {
            bol yaar("Zero se divide nahi hota!");
        }
        bas kar yaar;
    garmi:
        bol yaar("Invalid operator!");
}
```

### 3.6 Iteration Loops
#### A. For Loop (`ghoom yaar`)
```c
ghoom yaar (rakh yaar number i = 0; i < 5; i = i + 1) {
    bol yaar(i);
}
```

#### B. While Loop (`jab tak yaar`)
```c
rakh yaar number counter = 5;
jab tak yaar (counter > 0) {
    bol yaar(counter);
    counter = counter - 1;
}
```

#### C. Do-While Loop (`chala yaar ... jab tak yaar`)
```c
rakh yaar number n = 1;
chala yaar {
    bol yaar(n);
    n = n + 1;
} jab tak yaar (n <= 3);
```

### 3.7 Functions (`jugaad`) & Returns (`de yaar`)
```c
jugaad number add(number a, number b) {
    de yaar a + b;
}

jugaad bool checkPositive(number x) {
    agar yaar (x > 0) {
        de yaar pakka;
    } warna {
        de yaar topi;
    }
}
```

### 3.8 Arrays (`list yaar`)
```c
list yaar number marks[3];
marks[0] = 85;
marks[1] = 92;
marks[2] = 78;

rakh yaar number total = marks[0] + marks[1] + marks[2];
bol yaar(total);
```
