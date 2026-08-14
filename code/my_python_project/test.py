# print('mkbk')
# a = 10
# a = "ab"
# print(type(a))
# t = [1, 2, 3]
# b = t
# b.append(4)
# print(t)


# a = 88
# if a >= 90:
#     print('A')
# elif a >= 80:
#     print('B')
# else:
#     print('C')

# for i in range(4):
#     print(i)

# fruit = ["apple", "banana", "orange"]
# for item in fruit:
#     print(item)

# limit = 5
# for i in range(1, 11, 2):
#     if i > limit:
#         print(f"{i} 大于 {limit}")
#     else:
#         print(f"{i} 小于或等于 {limit}")


# student = [1, 2, 3, 4]
# student.append(5)
# print(student[0 : 5])
# print(student[-1])


# student_info = ("张三", "10086")
# # can not alter
# print(student_info[-1])
# print(student_info)

# fitness_score = {
#     "张三" : 99,
#     "李四" : 66
# }
# fitness_score["王五"] = 89
# print(fitness_score["王五"])
# print(fitness_score.get("赵六", "undefine"))

# score = [100, 99, 100, 87, 78, 99]
# print(score)
# score = set(score)
# print(score)


# raw_names = ["张三", "李四", "王五", "张三" ,"赵六", "李四"]
# new_names = set(raw_names)
# init_names = list(new_names)
# print(init_names)
# for name in init_names:
#     print(name)
# new_names.add("张三")
# print(new_names)


scores = [75, 42, 88, 59, 91, 35, 66]
passed_bonus_scores = [x + 5 for x in scores if x >= 60]
print(passed_bonus_scores)







