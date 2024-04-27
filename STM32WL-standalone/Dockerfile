# Utiliser une image de base Debian
FROM debian:stable-slim

# Mettre à jour les paquets et installer les outils de cross-compilation
RUN apt-get update && apt-get install -y \
    gcc-arm-none-eabi \
    make

# Définir le répertoire de travail dans le conteneur
WORKDIR /workspace

# Nettoyer les caches de paquets
RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# Définir CMD pour lancer make par défaut
CMD ["make"]
