# MIDL语言编译器（c++）

## 1. 项目介绍

本项目为学校编译原理课程的实验，通过c++编写。

实现了对MIDL语言的**词法分析**和**语法分析**功能，可输出对应的词法分析结果（**单词与单词类型列表**）和语法分析结果（**抽象语法树**）。语法分析算法采用了相对较为简单的递归下降算法实现。

同时，支持出错恢复机制来检查出更多的错误，并维护了较为完整的出错信息。



## 2. MIDL语言

### 2.1.MIDL词法规则

1）语言的关键字（16个）：

| struct   | float  | boolean | short  | long  |
| -------- | ------ | ------- | ------ | ----- |
| double   | int8   | int16   | int32  | int64 |
| unit8    | unit16 | unit32  | unit64 | char  |
| unsigned |        |         |        |       |

注：关键字是保留字，并且不区分大小写。

2）语言的专用符号17个：

| {    | }    | ;    | [    | ]    |
| ---- | ---- | ---- | ---- | ---- |
| *    | +    | -    | ~    | /    |
| %    | >>   | <<   | &    | ^    |
| \|   | ,    |      |      |      |

3）标示符ID、整数INTEGER、字符串STRING（支持转义字符）和BOOLEAN等4个词法规则通过下列正则表达式定义：

- **ID** =  LETTER (UNDERLINE?( LETTER | DIGIT))*

  LETTER = [a-z] | [A- Z]

  DIGIT = [0-9]

  UNDERLINE= _

- **INTEGER** = (0 | [1-9] [0-9]*) INTEGER_TYPE_SUFFIX?

  INTEGER_TYPE_SUFFIX =  l | L

- **STRING** =  " (ESCAPE_SEQUENCE |  (~\ | ~") )*  " 

  ESCAPE_SEQUENCE =  \ ( b | t | n | f | r |  " | \ )

- **BOOLEAN** =  TRUE | FALSE

### 2.2.MIDL语法规则

- struct_type->“struct” ID “{”   member_list “}”EOF

- member_list-> { type_spec declarators “;” }

- type_spec -> base_type_spec | struct_type

- base_type_spec->floating_pt_type|integer_type| “char” | “boolean”

- floating_pt_type -> “float” | “double” | “long double”

- integer_type -> signed_int | unsigned_int

- signed_int->(“short”|“int16”)

  |(“long”|“int32”)

  |(“long” “long”|“int64”)

  |“int8”

- unsigned_int -> (“unsigned”“short”| “unit16”)

     | (“unsigned”“long”| “unit32”)

     | (“unsigned” “long” “long” | “unit64”)

     | “unit8”

- declarators -> declarator {“,” declarator }

- declarator -> ID [ exp_list ]

- exp_list -> “[” or_expr { “,”or_expr } “]”

- or_expr -> xor_expr {“|” xor_expr }

- xor_expr -> and_expr {“^” and_expr }

- and_expr -> shift_expr {“&”shift_expr }

- shift_expr -> add_expr { (“>>” | “<<”) add_expr }

- add_expr -> mult_expr { (“+” | “-”) mult_expr }

- mult_expr -> unary_expr { (“*” |“/”|“%”) unary_expr }

- unary_expr -> [“-”| “+” | “~”] ( INTEGER | STRING |  BOOLEAN )