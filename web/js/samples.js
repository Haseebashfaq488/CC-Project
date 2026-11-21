// Preloaded code samples for the YaarLang Playground
const SAMPLES = {
    fibonacci: `sun yaar {
    rakh yaar number terms = 8;
    rakh yaar number t1 = 0;
    rakh yaar number t2 = 1;
    rakh yaar number nextTerm = 0;

    bol yaar("--- Fibonacci Sequence ---");

    ghoom yaar (rakh yaar number i = 1; i <= terms; i = i + 1) {
        agar yaar (i == 1) {
            bol yaar(t1);
            agay chal yaar;
        }
        agar yaar (i == 2) {
            bol yaar(t2);
            agay chal yaar;
        }

        nextTerm = t1 + t2;
        t1 = t2;
        t2 = nextTerm;

        bol yaar(nextTerm);
    }
}
khatam yaar`,

    calculator: `sun yaar {
    rakh yaar number a = 50;
    rakh yaar number b = 20;
    rakh yaar char op = '+';
    rakh yaar number ans = 0;

    // Switch case with sardi & garmi
    dekh yaar (op) {
        sardi '+':
            ans = a + b;
            bol yaar("Addition performed!");
            bas kar yaar;

        sardi '-':
            ans = a - b;
            bol yaar("Subtraction performed!");
            bas kar yaar;

        sardi '*':
            ans = a * b;
            bol yaar("Multiplication performed!");
            bas kar yaar;

        sardi '/':
            agar yaar (b != 0) {
                ans = a / b;
            } warna {
                bol yaar("Divide by zero error!");
            }
            bas kar yaar;

        garmi:
            bol yaar("Default: Unknown operator!");
    }

    bol yaar("Result is:");
    bol yaar(ans);
}
khatam yaar`,

    even_odd: `sun yaar {
    rakh yaar number num = 17;

    bol yaar("Checking number:");
    bol yaar(num);

    agar yaar (num % 2 == 0) {
        bol yaar("Number is Even (Scene Fit Hai)!");
    } warna {
        bol yaar("Number is Odd (Scene Ulta Hai)!");
    }
}
khatam yaar`,

    arrays: `sun yaar {
    list yaar number scores[5];

    scores[0] = 75;
    scores[1] = 88;
    scores[2] = 95;
    scores[3] = 62;
    scores[4] = 80;

    rakh yaar number total = 0;
    rakh yaar number maxScore = scores[0];

    ghoom yaar (rakh yaar number i = 0; i < 5; i = i + 1) {
        total = total + scores[i];

        agar yaar (scores[i] > maxScore) {
            maxScore = scores[i];
        }
    }

    bol yaar("Total Score:");
    bol yaar(total);
    bol yaar("Max Score:");
    bol yaar(maxScore);
}
khatam yaar`,

    functions: `sun yaar {
    jugaad bool checkEligibility(number age, bool hasId) {
        agar yaar (age >= 18 && hasId == pakka) {
            de yaar pakka;
        } warna {
            de yaar topi;
        }
    }

    rakh yaar number userAge = 21;
    rakh yaar bool userCnic = pakka;

    rakh yaar bool result = checkEligibility(userAge, userCnic);

    agar yaar (result == pakka) {
        bol yaar("Eligible for license (Pakka)!");
    } warna {
        bol yaar("Not eligible (Topi)!");
    }
}
khatam yaar`
};
