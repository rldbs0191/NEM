import matplotlib.pyplot as plt
import numpy as np
import os

def read_flux_file(filename):
    flux_data = []
    current_layer = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith("Z ="):
                if current_layer:
                    max_len = max(len(row) for row in current_layer)
                    padded = [row + [0.0] * (max_len - len(row)) for row in current_layer]
                    flux_data.append(np.array(padded, dtype=float))
                    current_layer = []
            elif line:
                values = [float(val) for val in line.split() if val.strip()]
                if values:
                    current_layer.append(values)
        if current_layer:
            max_len = max(len(row) for row in current_layer)
            padded = [row + [0.0] * (max_len - len(row)) for row in current_layer]
            flux_data.append(np.array(padded, dtype=float))
    return np.array(flux_data)

def plot_flux_2d_per_layer(flux_3d, group=1):
    os.makedirs(f'flux_group{group}_layers', exist_ok=True)
    for z in range(flux_3d.shape[0]):
        plt.figure(figsize=(6, 5))
        plt.imshow(flux_3d[z], cmap='viridis', origin='lower')
        plt.title(f'Flux Group {group} - Z = {z}')
        plt.colorbar(label='Flux')
        plt.xlabel('X')
        plt.ylabel('Y')
        plt.tight_layout()
        plt.savefig(f'flux_group{group}_layers/z{z}.png', dpi=300)
        plt.close()

# 실행
flux_array = read_flux_file('flux_group_1.txt')
plot_flux_2d_per_layer(flux_array, group=1)
flux_array = read_flux_file('flux_group_2.txt')
plot_flux_2d_per_layer(flux_array, group=2)
