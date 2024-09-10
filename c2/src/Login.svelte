<script>
  import { onMount } from 'svelte';
  import { navigate } from "svelte-routing";
  let username;
  let password;
  let csrfToken;

  // Fetch the CSRF token from the X-CSRFToken header
  const fetchCsrfToken = async () => {
    const response = await fetch("http://localhost:5000/api/csrf-token", {
      credentials: "include",  // Include cookies (session cookie) in the request
    });
    csrfToken = response.headers.get("X-CSRFToken");  // Extract the CSRF token from the headers
  };

  // Fetch the CSRF token when the component is mounted
  onMount(() => {
    fetchCsrfToken();
  });

  // Login function
  const login = () => {
    fetch("http://localhost:5000/api/login", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "X-CSRFToken": csrfToken,  // Include the CSRF token in the headers
      },
      credentials: "include",  // Include session cookies in the request
      body: JSON.stringify({ username, password }),
    })
    .then((res) => res.json())
    .then((data) => {
      if (data.login) {
        navigate("/authenticated");
      } else {
        alert("Login failed");
      }
    })
    .catch((err) => {
      console.error(err);
    });
  };
</script>

<div>
  <h1>Log in</h1>
  <form id="form">
    username:
    <input type="text" bind:value={username} />
    <br /><br />
    password:
    <input type="password" bind:value={password} /><br /><br />
    <button type="button" on:click={login}>Login</button>
  </form>
</div>
