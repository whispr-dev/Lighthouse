FROM python:3.11-slim

# Install supervisor and caddy
RUN apt-get update && apt-get install -y \
    supervisor \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install Caddy
RUN curl -1sLf 'https://dl.cloudsmith.io/public/caddy/stable/gpg.key' | gpg --dearmor -o /usr/share/keyrings/caddy-stable-archive-keyring.gpg
RUN curl -1sLf 'https://dl.cloudsmith.io/public/caddy/stable/debian.deb.txt' | tee /etc/apt/sources.list.d/caddy-stable.list
RUN apt-get update && apt-get install -y caddy

# Create app directory
WORKDIR /app

# Copy requirements and install Python dependencies (if any)
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application files
COPY litehaus-executable ./litehaus-executable
COPY rebroadcaster.py ./rebroadcaster.py
COPY static/ ./static/

# Make executable actually executable
RUN chmod +x ./litehaus-executable

# Copy configuration files
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY Caddyfile /etc/caddy/Caddyfile

# Create log directories
RUN mkdir -p /var/log/supervisor /var/log/litehaus

# Expose the port Caddy will use
EXPOSE 7502

# Start supervisor (which manages everything)
CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
