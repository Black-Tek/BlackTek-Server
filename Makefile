env:
	if [ ! -f .env ]; then cp .env.example .env; else echo ".env already exists. Skipping."; fi

compose: env
	docker-compose down -v
	docker volume prune -f
	docker-compose up --build -d
	docker logs server -f
