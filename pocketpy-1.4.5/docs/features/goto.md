---
icon: dot
title: Goto Statement
---

pkpy supports goto/label just like C. You are allowed to **change the control flow unconditionally**.

!!!
This feature is stable after v1.2.2.
!!!

## Define a label

```
== <identifier> ==
```

## Goto a label

```
-> <identifier>
```

## Example

```python
for i in range(10):
  for j in range(10):
    for k in range(10):
      -> exit

== exit ==
print('exit')
```
