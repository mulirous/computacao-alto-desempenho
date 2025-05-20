import numpy as np
import matplotlib.pyplot as plt
import glob
import os
import imageio


# Tamanho do domínio
NX = 64
NY = 64

# Pasta com os arquivos
arquivos = sorted(glob.glob("vel_*.dat"))

# Cria a grade de coordenadas
X, Y = np.meshgrid(np.arange(NX), np.arange(NY))

# Cria diretório para imagens (opcional)
os.makedirs("frames", exist_ok=True)

for arquivo in arquivos:
    # Carrega os dados: i, j, u, v
    data = np.loadtxt(arquivo)
    u = data[:, 2].reshape((NY, NX))
    v = data[:, 3].reshape((NY, NX))

    plt.figure(figsize=(6, 6))
    plt.quiver(X, Y, u, v, scale=5, pivot="mid", color='blue')
    plt.title(f"Campo de velocidade - {arquivo}")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.xlim(0, NX)
    plt.ylim(0, NY)
    plt.grid(True)

    # Salvar imagem
    frame_name = f"frames/{arquivo.replace('.dat', '.png')}"
    plt.savefig(frame_name)
    plt.close()

print("Imagens geradas em 'frames/'.")

# Cria GIF com os frames gerados
with imageio.get_writer("simulacao.gif", mode="I", duration=0.2) as writer:
    for arquivo in sorted(glob.glob("frames/vel_*.png")):
        image = imageio.imread(arquivo)
        writer.append_data(image)

print("GIF gerado como simulacao.gif")
