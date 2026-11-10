
import os
from decimal import Decimal, InvalidOperation

import MySQLdb
import flask 
from flask_mysqldb import MySQL
from dotenv import load_dotenv
load_dotenv()
from werkzeug.security import generate_password_hash, check_password_hash

load_dotenv()

app = flask.Flask(__name__)
app.secret_key = os.getenv("FLASK_SECRET_KEY", "change-this-development-secret")

# MySQL configuration
app.config["MYSQL_HOST"] = os.getenv("DB_HOST", "localhost")
app.config["MYSQL_USER"] = os.getenv("DB_USER", "root")
app.config["MYSQL_PASSWORD"] = os.getenv("DB_PASSWORD", "")
app.config["MYSQL_DB"] = os.getenv("DB_NAME", "clothing_store")
app.config["MYSQL_CURSORCLASS"] = "DictCursor"
app.config["MYSQL_CHARSET"] = "utf8mb4"

mysql = MySQL(app)


def get_json():
    return flask.request.get_json(silent=True) or {}


def error(message, status=400):
    return flask.jsonify({"success": False, "message": message}), status


def get_cart():
    return flask.session.get("cart", {})


# ---------------- HOME AND WEBSITE PAGES ----------------

@app.route("/")
def home():
    return flask.render_template("index.html")


@app.route("/shop")
def shop():
    return flask.render_template("shop.html")


@app.route("/about")
def about():
    return flask.render_template("about.html")


@app.route("/blog")
def blog():
    return flask.render_template("blog.html")


@app.route("/contact")
def contact():
    return flask.render_template("contact.html")


# ---------------- PRODUCTS API ----------------

@app.route("/api/products", methods=["GET"])
def get_products():
    search = flask.request.args.get("search", "").strip()
    category = flask.request.args.get("category", "").strip()

    try:
        cursor = mysql.connection.cursor()
        sql = """
            SELECT id, name, category, description, price,
                   image_url, stock
            FROM products
            WHERE 1=1
        """
        params = []

        if search:
            sql += " AND (name LIKE %s OR description LIKE %s)"
            params.extend([f"%{search}%", f"%{search}%"])

        if category:
            sql += " AND category = %s"
            params.append(category)

        sql += " ORDER BY created_at DESC"
        cursor.execute(sql, tuple(params))
        products = cursor.fetchall()
        cursor.close()

        for product in products:
            product["price"] = float(product["price"])

        return flask.jsonify({"success": True, "products": products})

    except MySQLdb.Error:
        app.logger.exception("Could not load products")
        return error("Unable to load products. Check the database.", 500)


# ---------------- SHOPPING CART ----------------

@app.route("/api/cart", methods=["GET"])
def view_cart():
    cart = get_cart()
    items = []
    total = Decimal("0.00")

    try:
        if cart:
            ids = [int(pid) for pid in cart.keys()]
            placeholders = ",".join(["%s"] * len(ids))

            cursor = mysql.connection.cursor()
            cursor.execute(
                f"""SELECT id, name, price, image_url, stock
                    FROM products WHERE id IN ({placeholders})""",
                tuple(ids)
            )
            products = {str(p["id"]): p for p in cursor.fetchall()}
            cursor.close()

            for pid, quantity in cart.items():
                product = products.get(str(pid))
                if not product:
                    continue

                price = Decimal(str(product["price"]))
                subtotal = price * int(quantity)
                total += subtotal

                items.append({
                    "id": product["id"],
                    "name": product["name"],
                    "price": float(price),
                    "image_url": product["image_url"],
                    "quantity": int(quantity),
                    "stock": int(product["stock"]),
                    "subtotal": float(subtotal)
                })

        return flask.jsonify({
            "success": True,
            "items": items,
            "total": float(total)
        })

    except (MySQLdb.Error, ValueError):
        app.logger.exception("Could not load cart")
        return error("Unable to load cart.", 500)


@app.route("/api/cart", methods=["POST"])
def add_to_cart():
    data = get_json()

    try:
        product_id = int(data.get("product_id", 0))
        quantity = int(data.get("quantity", 1))
        if product_id < 1 or quantity < 1:
            return error("Enter a valid product and quantity.")
    except (ValueError, TypeError):
        return error("Product ID and quantity must be numbers.")

    try:
        cursor = mysql.connection.cursor()
        cursor.execute(
            "SELECT id, stock FROM products WHERE id = %s",
            (product_id,)
        )
        product = cursor.fetchone()
        cursor.close()

        if not product:
            return error("Product not found.", 404)

        cart = get_cart()
        key = str(product_id)
        new_quantity = int(cart.get(key, 0)) + quantity

        if new_quantity > product["stock"]:
            return error("Requested quantity exceeds available stock.")

        cart[key] = new_quantity
        flask.session["cart"] = cart
        flask.session.modified = True

        return flask.jsonify({
            "success": True,
            "message": "Product added to cart.",
            "cart": cart
        })

    except MySQLdb.Error:
        app.logger.exception("Could not add product")
        return error("Unable to update cart.", 500)


@app.route("/api/cart/<int:product_id>", methods=["PATCH", "DELETE"])
def update_cart(product_id):
    cart = get_cart()
    key = str(product_id)

    if key not in cart:
        return error("Product is not in your cart.", 404)

    if flask.request.method == "DELETE":
        cart.pop(key, None)
    else:
        data = get_json()
        try:
            quantity = int(data.get("quantity", 1))
            if quantity < 1:
                return error("Quantity must be at least 1.")
        except (ValueError, TypeError):
            return error("Enter a valid quantity.")

        try:
            cursor = mysql.connection.cursor()
            cursor.execute(
                "SELECT stock FROM products WHERE id = %s",
                (product_id,)
            )
            product = cursor.fetchone()
            cursor.close()

            if not product:
                cart.pop(key, None)
                flask.session["cart"] = cart
                return error("Product not found.", 404)

            if quantity > product["stock"]:
                return error("Requested quantity exceeds available stock.")

            cart[key] = quantity
        except MySQLdb.Error:
            app.logger.exception("Could not update cart")
            return error("Unable to update cart.", 500)

    flask.session["cart"] = cart
    flask.session.modified = True
    return flask.jsonify({"success": True, "cart": cart})


# ---------------- CUSTOMER REGISTRATION ----------------

@app.route("/api/register", methods=["POST"])
def register():
    data = get_json()
    name = str(data.get("name", "")).strip()
    email = str(data.get("email", "")).strip().lower()
    password = data.get("password", "")

    if not name or not email or not isinstance(password, str):
        return error("Name, email and password are required.")

    if len(name) > 100 or len(email) > 150:
        return error("Name or email is too long.")

    if "@" not in email or len(password) < 8:
        return error("Enter a valid email and a password of at least 8 characters.")

    password_hash = generate_password_hash(password)

    try:
        cursor = mysql.connection.cursor()
        cursor.execute(
            """INSERT INTO customers (name, email, password_hash)
               VALUES (%s, %s, %s)""",
            (name, email, password_hash)
        )
        mysql.connection.commit()
        cursor.close()
        return flask.jsonify({"success": True, "message": "Registration successful."}), 201

    except MySQLdb.IntegrityError:
        mysql.connection.rollback()
        return error("This email is already registered.", 409)
    except MySQLdb.Error:
        mysql.connection.rollback()
        app.logger.exception("Registration failed")
        return error("Unable to register customer.", 500)


# ---------------- CUSTOMER LOGIN ----------------

@app.route("/api/login", methods=["POST"])
def login():
    data = get_json()
    email = str(data.get("email", "")).strip().lower()
    password = data.get("password", "")

    if not email or not isinstance(password, str):
        return error("Email and password are required.")

    try:
        cursor = mysql.connection.cursor()
        cursor.execute(
            "SELECT id, name, email, password_hash FROM customers WHERE email = %s",
            (email,)
        )
        customer = cursor.fetchone()
        cursor.close()

        if not customer or not check_password_hash(
            customer["password_hash"], password
        ):
            return error("Invalid email or password.", 401)

        flask.session.clear()
        flask.session["customer_id"] = customer["id"]
        flask.session["customer_name"] = customer["name"]

        return flask.jsonify({
            "success": True,
            "message": "Login successful.",
            "customer": {
                "id": customer["id"],
                "name": customer["name"],
                "email": customer["email"]
            }
        })

    except MySQLdb.Error:
        app.logger.exception("Login failed")
        return error("Unable to log in.", 500)


@app.route("/api/logout", methods=["POST"])
def logout():
    flask.session.clear()
    return flask.jsonify({"success": True, "message": "Logged out."})


# ---------------- CONTACT FORM ----------------

@app.route("/api/contact", methods=["POST"])
def submit_contact():
    data = get_json()

    name = str(data.get("name", "")).strip()
    email = str(data.get("email", "")).strip()
    phone = str(data.get("phone", "")).strip()
    subject = str(data.get("subject", "")).strip()
    message = str(data.get("message", "")).strip()

    if not all([name, email, subject, message]):
        return error("Name, email, subject and message are required.")

    if (len(name) > 100 or len(email) > 150 or len(phone) > 25
            or len(subject) > 100):
        return error("One or more fields exceed the allowed length.")

    if "@" not in email:
        return error("Enter a valid email address.")

    try:
        cursor = mysql.connection.cursor()
        cursor.execute(
            """INSERT INTO contact_messages
               (name, email, phone, subject, message)
               VALUES (%s, %s, %s, %s, %s)""",
            (name, email, phone or None, subject, message)
        )
        mysql.connection.commit()
        cursor.close()

        return flask.jsonify({
            "success": True,
            "message": "Your message has been sent successfully."
        }), 201

    except MySQLdb.Error:
        mysql.connection.rollback()
        app.logger.exception("Contact form submission failed")
        return error("Unable to save your message.", 500)


# ---------------- PLACE AN ORDER ----------------

@app.route("/api/checkout", methods=["POST"])
def checkout():
    customer_id = flask.session.get("customer_id")
    if not customer_id:
        return error("Please log in before placing an order.", 401)

    cart = get_cart()
    if not cart:
        return error("Your cart is empty.")

    try:
        cursor = mysql.connection.cursor()
        total = Decimal("0.00")
        order_lines = []

        # Lock product rows and check stock before placing the order.
        for pid, quantity in cart.items():
            quantity = int(quantity)
            cursor.execute(
                "SELECT id, price, stock FROM products WHERE id = %s FOR UPDATE",
                (int(pid),)
            )
            product = cursor.fetchone()

            if not product or quantity < 1 or quantity > product["stock"]:
                mysql.connection.rollback()
                cursor.close()
                return error("A product is unavailable or has insufficient stock.")

            price = Decimal(str(product["price"]))
            total += price * quantity
            order_lines.append((product["id"], quantity, price))

        cursor.execute(
            "INSERT INTO orders (customer_id, total, status) VALUES (%s, %s, %s)",
            (customer_id, total, "Pending")
        )
        order_id = cursor.lastrowid

        for product_id, quantity, price in order_lines:
            cursor.execute(
                """INSERT INTO order_items
                   (order_id, product_id, quantity, unit_price)
                   VALUES (%s, %s, %s, %s)""",
                (order_id, product_id, quantity, price)
            )
            cursor.execute(
                "UPDATE products SET stock = stock - %s WHERE id = %s",
                (quantity, product_id)
            )

        mysql.connection.commit()
        cursor.close()

        flask.session["cart"] = {}
        return flask.jsonify({
            "success": True,
            "message": "Order created successfully. Payment has not been collected.",
            "order_id": order_id,
            "total": float(total)
        }), 201

    except (MySQLdb.Error, ValueError, InvalidOperation):
        mysql.connection.rollback()
        app.logger.exception("Checkout failed")
        return error("Unable to place order.", 500)


# ---------------- RUN SERVER ----------------

if __name__ == "__main__":
    app.run(debug=True)