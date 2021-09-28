import cv2

class Dog:
    def __init__(self, name, age):
        self.name = name
        self.age = 12


if __name__ == "__main__":
    #dog = Dog("bob", 12)
    print(cv2.__version__)

    dog = None
    print(dog.height if hasattr(dog,'height') else "ko")

    tupla = [(1,0), (2,2), (6,8)]
    print(tupla[2][1])
