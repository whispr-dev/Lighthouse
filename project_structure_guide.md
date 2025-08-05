# LiteHaus Docker Deployment Structure

Here's how to organize your deployment directory:

```
litehaus-deploy/
├── Dockerfile
├── docker-compose.yml
├── supervisord.conf
├── Caddyfile
├── requirements.txt
├── deploy.sh
├── litehaus-executable          # Your pre-compiled executable
├── rebroadcaster.py            # Your Python rebroadcaster script
├── static/                     # Any static files (CSS, JS, images)
│   └── (your static assets)
└── logs/                       # Will be created automatically
    └── (log files appear here)
```

## Quick Setup Steps:

1. **Copy your files:**
   - Put your `litehaus-executable` in the root directory
   - Put your `rebroadcaster.py` in the root directory
   - Add any Python dependencies to `requirements.txt`

2. **Adjust ports in Caddyfile:**
   - Check what ports your executable and Python script use
   - Update the `reverse_proxy` lines accordingly
   - Default assumes executable on :8080, Python on :8000

3. **Deploy:**
   ```bash
   chmod +x deploy.sh
   ./deploy.sh
   ```

4. **Access your litehaus:**
   - Local: `http://localhost:7502`
   - Remote: `http://YOUR_DROPLET_IP:7502`

## Customization Notes:

- **Port Changes:** Edit the Caddyfile if your services run on different ports
- **Dependencies:** Add Python packages to requirements.txt as needed
- **Static Files:** Put CSS/JS/images in the `static/` directory
- **SSL/Domain:** Caddy can auto-handle HTTPS if you use a domain instead of :7502

## Monitoring:

- View all logs: `docker-compose logs -f`
- Check individual services: `docker-compose logs -f litehaus`
- Access container shell: `docker-compose exec litehaus /bin/bash`
