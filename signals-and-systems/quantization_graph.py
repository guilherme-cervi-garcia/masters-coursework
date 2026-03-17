import matplotlib.pyplot as plt
import numpy as np
from IPython.display import Image, display

# Criando os dados
x = np.linspace(0, 10, 500)  # Eixo X contínuo
analog_signal = x  # Sinal analógico contínuo
sampling_points = np.linspace(0, 10, 11)  # Pontos de amostragem
quantized_signal = np.round(sampling_points)  # Sinal quantizado

# Ajustando as dimensões dos dados para o gráfico em degraus
step_x = np.repeat(sampling_points, 2)
step_y = np.repeat(np.round(sampling_points), 2)

# Removendo os primeiros e últimos pontos extras para alinhar os degraus
step_x = np.hstack(([step_x[0]], step_x, [step_x[-1]]))
step_y = np.hstack(([step_y[0]], step_y, [step_y[-1]]))

# Recriando o gráfico
plt.figure(figsize=(10, 6))
plt.plot(x, analog_signal, label="Sinal analógico (contínuo)", color="blue", linewidth=2)
plt.step(step_x, step_y, label="Sinal quantizado (degraus)", color="orange", where='mid', linewidth=2)
plt.scatter(sampling_points, quantized_signal, color="red", zorder=5, label="Amostras quantizadas")

# Configurações do gráfico
plt.title("Erro de Quantização - Representação Analógica vs Digital", fontsize=14)
plt.xlabel("Tempo (ou qualquer unidade contínua)", fontsize=12)
plt.ylabel("Amplitude", fontsize=12)
plt.grid(alpha=0.3)
plt.legend(fontsize=12)
plt.tight_layout()

# Salvando e exibindo o gráfico corrigido
image_path = "/mnt/data/1709387943.png"
plt.savefig(image_path)
plt.close()

# Exibindo a imagem
display(Image(filename=image_path))

