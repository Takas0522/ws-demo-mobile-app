FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Install basic tools
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    curl \
    wget \
    git \
    build-essential \
    ca-certificates \
    gnupg \
    lsb-release \
    sqlite3 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# === 追加: JDK 7 (Azul Zulu 7) ===
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    apt-transport-https \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && curl -fsSL https://repos.azul.com/azul-repo.key | gpg --dearmor -o /usr/share/keyrings/azul.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/azul.gpg] https://repos.azul.com/zulu/deb stable main" > /etc/apt/sources.list.d/zulu.list \
    && apt-get update \
    && apt-get -y install --no-install-recommends zulu7-jdk \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# === 追加: Apache Tomcat 7 ===
ENV CATALINA_HOME=/opt/tomcat7
RUN mkdir -p ${CATALINA_HOME} \
    && curl -fsSL https://archive.apache.org/dist/tomcat/tomcat-7/v7.0.109/bin/apache-tomcat-7.0.109.tar.gz \
    | tar xz --strip-components=1 -C ${CATALINA_HOME} \
    && chmod +x ${CATALINA_HOME}/bin/*.sh \
    && chmod -R a+rw ${CATALINA_HOME}/logs ${CATALINA_HOME}/work ${CATALINA_HOME}/temp ${CATALINA_HOME}/webapps

# Create non-root user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

USER $USERNAME

WORKDIR /workspace
