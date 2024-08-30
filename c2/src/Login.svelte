<script>
  import { onMount } from "svelte";
  import { navigate } from "svelte-routing";

  let username;
  let password;
  let csrf = document.getElementsByName("csrf-token")[0]?.content;

  const login = () => {
    fetch("/api/login", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "X-CSRFToken": csrf,
      },
      credentials: "same-origin",
      body: JSON.stringify({ username, password }),
    })
    .then((res) => res.json())
    .then((data) => {
      if (data.login === true) {
        navigate("/authenticated");
      }
    })
    .catch((err) => {
      console.log(err);
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
    <button type="button" on:click={login}>login</button>
  </form>
</div>

