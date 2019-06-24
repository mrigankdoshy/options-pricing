
import random
import matplotlib.pyplot as plt

circlePoints = 0
outsidePoints = 0
pi = []

runs = int(input('Number of runs to perform: '))
for i in range(runs):
    x = random.uniform(-1, 1)
    y = random.uniform(-1, 1)
    if (x ** 2 + y ** 2)  <= 1:
        circlePoints += 1
    else:
        outsidePoints += 1
    pi.append((4 * circlePoints) / (circlePoints + outsidePoints))
plt.plot(pi)
plt.ylabel('Estimated Pi')
plt.xlabel('Attempts')
plt.show()
pi = (4 * circlePoints) / (circlePoints + outsidePoints)
print('Estimated value of π =', pi)
