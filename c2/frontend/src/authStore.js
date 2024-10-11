// src/authStore.js
import { writable } from 'svelte/store';
import { navigate } from 'svelte-routing';

export const isAuthenticated = writable(false);
export const user = writable({});

export const checkAuth = async () => {
  try {
    const res = await fetch("http://localhost:5000/api/getsession", {
      credentials: "include",
    });
    const sessionData = await res.json();
    if (sessionData.login) {
      isAuthenticated.set(true);
      const userRes = await fetch("http://localhost:5000/api/data", {
        credentials: "include",
      });
      const userData = await userRes.json();
      user.set(userData);
    } else {
      isAuthenticated.set(false);
      user.set(null);
      navigate("/login");
    }
  } catch (err) {
    console.error(err);
    isAuthenticated.set(false);
    user.set(null);
    navigate("/login");
  }
};

// Function to log out
export const logout = async () => {
  try {
    await fetch("http://localhost:5000/api/logout", {
      method: "POST",
      credentials: "include",
    });
    isAuthenticated.set(false);
    user.set(null);
    navigate("/login");
  } catch (err) {
    console.error(err);
  }
};

