FROM debian:sid
RUN apt-get update \
 && apt-get install --yes --no-install-recommends \
      build-essential \
      ca-certificates \
      dos2unix \
      g++-10 \
      wget \
      unzip \
 && rm -rf /var/lib/apt/lists/*
COPY . /src
WORKDIR /src
CMD ["/src/test"]
