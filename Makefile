.PHONY: compose env clean

compose: env
	docker-compose down -v
	docker volume prune -f
	docker-compose up --build -d
	docker logs server -f

env:
	@ if [ ! -f .env ]; then cp .env.example .env; fi

clean:
	docker-compose down -v
	docker system prune -a --volumes -f
