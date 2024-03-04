# **MakeBigStringMicrosoftCompliant**
Splits a big string to smaller 2048 byte string literals for [Microsoft Compliance](https://learn.microsoft.com/en-us/cpp/c-language/maximum-string-length?view=msvc-170).

## **Usage**
`./makestringmc.exe <inputFile> <outputFile>`

## **Sample Input and Output**

**inputFile Contents:** "A…B…C…D…"

**outputFile Contents:** std::string(R"(A…)") + std::string(R"(B…)") + std::string(R"(C…)") + std::string(R"(D…)");
