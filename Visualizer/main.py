import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# 파일 불러오기
with open("flux_group_1.txt", "r") as f:
    lines = f.readlines()

# 유효한 줄만 필터링
filtered_lines = []
for line in lines:
    try:
        values = [float(x.replace('e', 'E')) for x in line.split()]
        if len(values) >= 10:
            filtered_lines.append(values)
    except ValueError:
        continue

# 가장 짧은 줄 기준 자르기
min_len = min(len(row) for row in filtered_lines)
trimmed_array = np.array([row[:min_len] for row in filtered_lines])

# 정규화
norm_flux_array = trimmed_array / np.max(trimmed_array)

# 애니메이션 생성
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)
ax.set_ylim(0, 1.1)
ax.set_xlim(0, norm_flux_array.shape[1])
ax.set_title("Flux Distribution per Layer")
ax.set_xlabel("Position")
ax.set_ylabel("Normalized Flux")

def init():
    line.set_data([], [])
    return line,

def animate(i):
    x = np.arange(norm_flux_array.shape[1])
    y = norm_flux_array[i]
    line.set_data(x, y)
    ax.set_title(f"Flux Distribution - Layer {i + 1}")
    return line,

ani = animation.FuncAnimation(fig, animate, init_func=init, frames=norm_flux_array.shape[0], blit=True)

# gif로 저장
ani.save("flux_distribution.gif", writer="pillow", fps=5)
