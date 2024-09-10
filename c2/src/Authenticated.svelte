<script>
  import { onMount } from "svelte";
  import { navigate } from "svelte-routing";

  let isAuthenticated = false;
  let username = '';

  // Check if the user is authenticated
  onMount(() => {
    fetch("http://localhost:5000/api/getsession", {
      credentials: "include",  // Include session cookies in the request
    })
    .then((res) => res.json())
    .then((data) => {
      if (data.login) {
        isAuthenticated = true;
        fetch("http://localhost:5000/api/data", {
          credentials: "include",  // Include session cookies in the request
        })
        .then(res => res.json())
        .then(data => {
          username = data.username;
        });
      } else {
        navigate("/login");
      }
    })
    .catch((err) => {
      console.error(err);
      navigate("/login");
    });
  });

  const logout = () => {
    fetch("http://localhost:5000/api/logout", {
      method: "POST",
      credentials: "include",  // Include session cookies in the request
    })
    .then(() => {
      isAuthenticated = false;
      navigate("/login");
    })
    .catch((err) => {
      console.error(err);
    });
  };
</script>

<div>
  {#if isAuthenticated}
    <h1>Welcome, {username}!</h1>
    <button on:click={logout}>Logout</button>
  {/if}
</div>
