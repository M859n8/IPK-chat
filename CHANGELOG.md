# Changelog

The main project issue is the lack of UDP protocol support. TCP protocol works correctly.

- Initial commit [9466d6e](9466d6e46afb6a08b120ca3d86804c1db623be29)
- feat: makefile and parsing args [de40502](de40502b8f3ffe3d228b376e675482b07793b9d1)
Parsing arguments, their validation, and data structure creation.
- feat: message transformation for tcp [4274bed](4274bed22e9c704bbcdc4b02d9aa041be8bc9e5a)
Handling user-input messages.
- feat : simple machine states [a0581ed](a0581ed71492da5cb708e1730150c2f861a72d42)
Added handling of messages from the server. Implementation of the main program states in main.c.
- feat: processing hostname [238ad1a](238ad1ad1cee790fb52b34a8ab8440ee45de5031)
Minor fixes in state transitions. Calculation of the host's IP address from its name. Correction of variable display filling.
- feat: processing cntl+c [5631484](5631484536917bbc947b8973e782280811f5607d)
Handling the Ctrl+C signal. Removal of the 'output' variable in some message handling functions.
- feat: added comments [1559c3e](1559c3effe268c04b94123f2126394f1d242e495)

- feat: smal fix and documentation [87efb1d](87efb1d9d3960314805d39df8cd0bb089f2d0595)
Correction of error output when incorrect program arguments are entered.
- feat: added img [e3822a7](e3822a7790d36cb0d7b7b7cbed847410881a7c01)

- fix: readme [f244697](f244697a6e19e705a8d5107511edeca1fbd77e50)

- fix: final readme [14305e9](14305e9a5161dd120267d7a5e12f98c3fd4375f0)