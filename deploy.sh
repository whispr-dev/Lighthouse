#!/bin/bash

# Simple deployment script for litehaus
echo "🏠 Deploying LiteHaus Stack..."

# Create logs directory
mkdir -p logs

# Build and start the container
echo "🔨 Building container..."
docker-compose build

echo "🚀 Starting services..."
docker-compose up -d

echo "📊 Checking status..."
docker-compose ps

echo "🎯 LiteHaus should be available at:"
echo "   http://localhost:7502"
echo "   http://YOUR_DROPLET_IP:7502"

echo ""
echo "📝 Useful commands:"
echo "   View logs: docker-compose logs -f"
echo "   Stop: docker-compose down"
echo "   Restart: docker-compose restart"
echo "   Shell access: docker-compose exec litehaus /bin/bash"

echo ""
echo "✅ Deployment complete!"
