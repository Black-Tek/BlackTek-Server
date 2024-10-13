env:
	if [ ! -f .env ]; then cp .env.example .env; else echo ".env already exists. Skipping."; fi

compose:
	docker-compose down
	docker volume prune -f
	docker-compose up --build -d
	docker logs server -f
