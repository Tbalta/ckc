# Introduction
CKC is a programming language that I am developing for fun and to explore the compile-time programming paradigm. 

The name CKC is a French play on words "C'est cassé" which means "It's broken" in English. (The name will eventually change)

The language is inspired by C, C++ and Ada. Everyone is welcome to contribute to the project and propose their ideas.


# Why ?
Low-level programming often requires great control over memory and the assembly generated. The C language is a great tool, however, it reaches its limit in expressiveness. C++ addresses this problem by adding features such as templates and classes, but it is not the most user-friendly language to start with.

Ada provides unique functionalities such as subtyping and contract programming. When programming [AdOS](https://github.com/Tbalta/AdOS), I found myself struggling to understand what was going on under the hood (surely due to my lack of knowledge in ADA). I also believe that ADA relies too much on its standard library, and the distinction between the language and the standard library is vague.


# Goal
The goal of this project is to create a language that is easy to use, with a lot of compile-time features, while providing full control over the assembly generated.

Ideally, the language should be powerful enough to be used to write a small operating system.

# How ?
The file [specification.md](specification.md) will contain objective specifications of the language. This file describes general ideas and guidelines for the language, and will serve as a guide for the [roadmap](roadmap.md).

The project will be split into different milestones called "steps". These steps are described in [roadmap](roadmap.md). A step is a self-contained project, and at each step, the compiler should be able to compile a subset of the language.

## Steps
The grammar between each step is independent. This means that the compiler at the n+1 step will not necessarily be able to compile the code of the nth step. 

The features of the steps are incremental. This means that the features of the nth step will also be available in the n+1 step with rare exceptions.

During the development phase, the specification of the current step may be changed as problems arise.



